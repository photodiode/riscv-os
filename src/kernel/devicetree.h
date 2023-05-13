
#ifndef devicetree_h
#define devicetree_h

#include <types.h>


typedef struct {
	u32 offset; // 4 byte (u32) offset into the dtb's tree struct

	u8 address; // #address-cells (number of u32 cells)
	u8 length;  // #size-cells (number of u32 cells)

	char* name; // pointer to the name string
} dt_node;


typedef struct {
	u32 offset; // 4 byte (u32) offset into the dtb's tree struct

	u32 data_len;
	u8* data; // pointer to start of value data all in big endian

	char* name; // pointer to the name string
} dt_prop;


void dt_init(u64 dtb_address);
void dt_print(void);

u32 dt_find_node(u32 i, const char* name, u32 hit_num);
u32 dt_count_nodes(u32 i, const char* name);

u32 dt_get_prop(u32 node, const char* name);

bool dt_parse_node(u32 i, dt_node*);
bool dt_parse_prop(u32 i, dt_prop*);


#endif // devicetree_h
