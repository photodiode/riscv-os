
#ifndef devicetree_h
#define devicetree_h

#include <types.h>


typedef struct {
	u32 error  :  1;
	u32 offset : 31; // 4 byte (u32) offset into the dtb's tree struct

	u16 address_cells   : 4; // #address-cells (number of u32 cells)
	u16 size_cells      : 4; // #size-cells (number of u32 cells)
	u16 interrupt_cells : 4; // #interrupt-cells (number of u32 cells)

	char* name; // pointer to the name string
} dt_node;

typedef struct {
	u32 error  :  1;
	u32 offset : 31; // 4 byte (u32) offset into the dtb's tree struct

	u16 data_len;
	u8* data;

	char* name; // pointer to the name string
} dt_prop;


dt_node dt_init(u64 dtb_address, u64* size);
//void dt_print(void);

dt_node dt_get_node(dt_node parent, const char* name, u32 hit_num);
u32     dt_count_nodes(dt_node parent, const char* name);

dt_prop dt_get_prop(dt_node parent, const char* name);

bool dt_parse_node(u32 i, dt_node*);
bool dt_parse_prop(u32 i, dt_prop*);


#endif // devicetree_h
