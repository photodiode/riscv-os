
#include <types.h>
#include <bytes.h>
#include <str.h>

#include "devicetree.h"
#include "print.h"


#define CEIL_DIV(A, B) ((A > 0) ? (1 + ((A - 1) / B)) : 0)


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


static u32 dtb_len = 0;
static u8* dtb     = NULL;

static u32  dtb_strings_len = 0;
static u8*  dtb_strings     = NULL;

static u32  dtb_struct_len  = 0;
static u32* dtb_struct      = NULL;


void dt_init(u64 dtb_address) {

	dtb = (void*)dtb_address;

	u32* header = (void*)dtb;

	if (swap_u32(header[0]) != 0xd00dfeed) {
		fatal("Device Tree corrupted or missing\n");
	}

	if (swap_u32(header[5]) != 17) {
		fatal("Unsupported Device Tree version\n");
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

}


static u32 next_token(u32 i, u32* type) {

	if (i < dtb_struct_len) {
		*type = swap_u32(dtb_struct[i]);
		switch (*type) {
			case DT_NODE: {
				u64 len = strlen((char*)&dtb_struct[i+1]) + 1;
				return i + 1 + CEIL_DIV(len, 4);
			}
			case DT_NODE_END: {
				return i + 1;
			}
			case DT_PROPERTY: {
				u32 len = swap_u32(dtb_struct[i+1]);
				return i + 3 + CEIL_DIV(len, 4);
			}
			case DT_NOP: {
				return i + 1;
			}
			default: break;
		}
	}

	return 0;
}


u32 dt_find_node(u32 i, const char* name, u32 hit_num) {

	if (!name) return 0;

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
						return i;
					}
					hit += 1;
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

	return 0;
}

u32 dt_count_nodes(u32 i, const char* name) {

	if (!name) return 0;

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

u32 dt_get_prop(u32 i, const char* name) {

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
				if (indent == 0) return 0;
				break;
			}
			case DT_PROPERTY: {
				u32 offset = swap_u32(dtb_struct[i+2]);
				char* prop_name = (char*)&dtb_strings[offset];

				if (hash == strhash(prop_name) && indent == 1) {
					return i;
				}
			}
			default: break;
		}
		i = next;
	}

	return 0;
}


bool dt_parse_prop(u32 i, dt_prop* prop) {
	if (!i) return false;
	if (swap_u32(dtb_struct[i]) != DT_PROPERTY) return false;

	u32 str_offset = swap_u32(dtb_struct[i+2]);

	prop->offset = i;
	prop->name   = (char*)&dtb_strings[str_offset];

	prop->data_len = swap_u32(dtb_struct[i+1]);
	prop->data     = (u8*)&dtb_struct[i+3];

	return true;
}


bool dt_parse_node(u32 i, dt_node* node) {
	if (swap_u32(dtb_struct[i]) != DT_NODE) return false;

	node->offset = i;
	node->name   = (char*)&dtb_struct[i+1];

	node->address = 2;
	node->length  = 1;

	dt_prop prop;

	if (dt_parse_prop(dt_get_prop(i, "#address-cells"), &prop)) {
		node->address = swap_u32(*(u32*)prop.data);
	}

	if (dt_parse_prop(dt_get_prop(i, "#size-cells"), &prop)) {
		node->length = swap_u32(*(u32*)prop.data);
	}

	return true;
}


void dt_print(void) {

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
}
