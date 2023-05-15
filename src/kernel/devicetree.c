
#include <types.h>
#include <bytes.h>
#include <str.h>

#include "devicetree.h"


static inline u64 ceil_div(u64 a, u64 b) {
	return ((a > 0) ? (1 + ((a - 1) / b)) : 0);
}


typedef enum {
	DT_UNDEFINED = 0x00,
	DT_NODE      = 0x01,
	DT_NODE_END  = 0x02,
	DT_PROPERTY  = 0x03,
	DT_NOP       = 0x04,
	DT_END       = 0x09
} dt_node_type;

#define ROOT_HASH 0x1505

#define ADDRESS_CELLS   0x5871b80e // number of u32 cells for address
#define SIZE_CELLS      0x252a60a3 // number of u32 cells for length
#define INTERRUPT_CELLS 0x39471b15 // number of u32 cells for interrupt specifier
#define GPIO_CELLS      0x7579e6b7 // number of u32 cells for ???

#define DT_NODE_ERROR (dt_node){.error = 1}
#define DT_PROP_ERROR (dt_prop){.error = 1}


static u32 dtb_len = 0;
static u8* dtb     = NULL;

static u32  dtb_strings_len = 0;
static u8*  dtb_strings     = NULL;

static u32  dtb_struct_len  = 0;
static u32* dtb_struct      = NULL;


static u32 next_token(u32 i, u32* type) {

	if (i < dtb_struct_len) {
		*type = swap_u32(dtb_struct[i]);
		switch (*type) {
			case DT_NODE: {
				u64 len = strlen((char*)&dtb_struct[i+1]) + 1;
				return i + 1 + ceil_div(len, 4);
			}
			case DT_NODE_END: {
				return i + 1;
			}
			case DT_PROPERTY: {
				u32 len = swap_u32(dtb_struct[i+1]);
				return i + 3 + ceil_div(len, 4);
			}
			case DT_NOP: {
				return i + 1;
			}
			default: break;
		}
	}

	return 0;
}


dt_node dt_init(u64 dtb_address) {

	dtb = (void*)dtb_address;

	u32* header = (void*)dtb;

	if (swap_u32(header[0]) != 0xd00dfeed) {
		return DT_NODE_ERROR;
	}

	if (swap_u32(header[5]) != 17) {
		return DT_NODE_ERROR;
	}

	dtb_len = swap_u32(header[1]);

	dtb_strings_len = swap_u32(header[8]);
	dtb_strings     = &dtb[swap_u32(header[3])];

	dtb_struct_len = swap_u32(header[9]) / 4;
	dtb_struct     = (void*)&dtb[swap_u32(header[2])];

	// memory reservation map
	/*u64* dtb_mem_rsvmap = (void*)&dtb[swap_u32(header[4])];

	while (1) {
		static u32 i = 0;

		u64 address = swap_u64(dtb_mem_rsvmap[i + 0]);
		u64 size    = swap_u64(dtb_mem_rsvmap[i + 1]);

		if (address == 0 && size == 0) break;

		printf("|  0x%x bytes\n", address);
		printf("|  0x%x\n\n", size);

		i += 2;
	}*/
	// ----

	dt_node root = (dt_node){0};

	dt_prop address_cells = dt_get_prop(root, "#address-cells");
	dt_prop size_cells    = dt_get_prop(root, "#size-cells");

	root.address_cells = (address_cells.error) ? 2 : swap_u32(dtb_struct[address_cells.offset+3]);
	root.size_cells    = (size_cells.error)    ? 1 : swap_u32(dtb_struct[size_cells.offset+3]);

	return root;
}


dt_prop dt_get_prop(dt_node parent, const char* name) {

	if (parent.error) return DT_PROP_ERROR;
	if (!name) return DT_PROP_ERROR;

	u32 i = parent.offset;
	u32 type = 0;
	u32 next;

	i8 indent = 0;

	u32 hash = strhash(name);

	while ((next = next_token(i, &type))) {
		switch (type) {
			case DT_NODE: {
				indent += 1;
				break;
			}
			case DT_NODE_END: {
				indent -= 1;
				if (indent == 0) return DT_PROP_ERROR;
				break;
			}
			case DT_PROPERTY: {
				u32 offset = swap_u32(dtb_struct[i+2]);
				char* prop_name = (char*)&dtb_strings[offset];

				if (hash == strhash(prop_name) && indent == 1) {
					return (dt_prop){
						.offset = i,
						.name   = prop_name,

						.address_cells = parent.address_cells,
						.size_cells    = parent.size_cells,

						.data_len = swap_u32(dtb_struct[i+1]),
						.data     = (u8*)&dtb_struct[i+3]
					};
				}
			}
			default: break;
		}
		i = next;
	}

	return DT_PROP_ERROR;
}


dt_node dt_get_node(dt_node parent, const char* name, u32 hit_num) {

	if (parent.error) return DT_NODE_ERROR;
	if (!name) return DT_NODE_ERROR;

	u32 i = parent.offset;
	u32 type = 0;
	u32 next = 0;

	i8  indent = 0;
	u32 hit    = 0;

	u32 name_len = strlen(name);
	u32 hash     = strhash(name);

	while ((next = next_token(i, &type))) {
		switch (type) {
			case DT_NODE: {
				char* node_name = (char*)&dtb_struct[i+1];
				u32   node_hash;

				if (name[name_len-1] == '@') {
					node_hash = strnhash(name_len, node_name);
				} else {
					node_hash = strhash(node_name);
				}

				if (hash == node_hash && indent == 1) {
					if (hit == hit_num) {
						dt_node node = (dt_node){
							.offset = i,
							.name   = node_name
						};

						dt_prop address_cells = dt_get_prop(node, "#address-cells");
						dt_prop size_cells    = dt_get_prop(node, "#size-cells");

						node.address_cells = (address_cells.error) ? parent.address_cells : swap_u32(dtb_struct[address_cells.offset+3]);
						node.size_cells    = (size_cells.error)    ? parent.size_cells    : swap_u32(dtb_struct[size_cells.offset+3]);

						return node;
					}
					hit += 1;
				}

				indent += 1;
				break;
			}
			case DT_NODE_END: {
				indent -= 1;
				if (indent < 0) return DT_NODE_ERROR;
				break;
			}
			default: break;
		}
		i = next;
	}

	return DT_NODE_ERROR;
}

u32 dt_count_nodes(dt_node parent, const char* name) {

	if (parent.error) return 0;
	if (!name) return 0;

	u32 i = parent.offset;
	u32 type = 0;
	u32 next = 0;

	i8  indent = 0;
	u32 count  = 0;

	u32 name_len = strlen(name);
	u32 hash     = strhash(name);

	while ((next = next_token(i, &type))) {
		switch (type) {
			case DT_NODE: {
				char* node_name = (char*)&dtb_struct[i+1];
				u32   node_hash;

				if (name[name_len-1] == '@') {
					node_hash = strnhash(name_len, node_name);
				} else {
					node_hash = strhash(node_name);
				}

				if (hash == node_hash && indent == 1) {
					count += 1;
				}

				indent += 1;
				break;
			}
			case DT_NODE_END: {
				indent -= 1;
				if (indent < 0) return 0;
				break;
			}
			default: break;
		}
		i = next;
	}

	return count;
}


/*void dt_print(void) {

	if (!dtb) fatal("Device Tree not initialized\n");

	u32 i = 0;
	u32 type = 0;
	u32 next;

	u8 indent = 0;
	char tabs[16] = {'\0'};

	while ((next = next_token(i, &type))) {
		switch (type) {
			case DT_NODE: {
				char* name = (indent > 0) ? (char*)&dtb_struct[i+1] : "Device Tree";
				printf("%s%s {\n", tabs, name);
				tabs[indent] = '\t';
				indent += 1;
				break;
			}
			case DT_NODE_END: {
				indent -= 1;
				tabs[indent] = '\0';
				printf("%s}\n", tabs);
				break;
			}
			case DT_PROPERTY: {
				u32 len    = swap_u32(dtb_struct[i+1]);
				u32 offset = swap_u32(dtb_struct[i+2]);
				//printf("%s- %s: %s (%d)\n", tabs, &dtb_strings[offset], (u8*)&dtb_struct[i+3], len);
				printf("%s- %s: %d bytes\n", tabs, &dtb_strings[offset], len);
				break;
			}
			default: break;
		}
		i = next;
	}
}*/
