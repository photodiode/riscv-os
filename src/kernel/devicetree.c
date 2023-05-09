
#include <types.h>

#include "print.h"


u32 swap_u32(u32 v) {
	return (
		((v & (0xffU<< 0)) << 24) |
		((v & (0xffU<< 8)) <<  8) |
		((v & (0xffU<<16)) >>  8) |
		((v & (0xffU<<24)) >> 24)
	);
}

u64 swap_u64(u64 v) {
	return (
		((v & (0xffUL<< 0)) << 56) |
		((v & (0xffUL<< 8)) << 48) |
		((v & (0xffUL<<16)) << 40) |
		((v & (0xffUL<<24)) << 32) |
		((v & (0xffUL<<32)) >> 32) |
		((v & (0xffUL<<40)) >> 40) |
		((v & (0xffUL<<48)) >> 48) |
		((v & (0xffUL<<56)) >> 56)
	);
}


u64 strlen(const u8* s) {
	u64 len = 0;
	while (*s++ != '\0') len++;
	return len;
}


#define CEIL_DIV(A, B) ((A > 0) ? (1 + ((A - 1) / B)) : 0)


void dt_parse(u64 dtb_address) {

	u8*  dtb    = (void*)dtb_address;
	u32* header = (void*)dtb;

	if (swap_u32(header[0]) != 0xd00dfeed) {
		fatal("Device Tree corrupted or missing\n");
	}

	if (swap_u32(header[5]) != 17) {
		fatal("Unsupported Device Tree version\n");
	}

	//u32 dtb_len = swap_u32(header[1]);

	// memory reservation map
	u64* dtb_mem_rsvmap = (void*)&dtb[swap_u32(header[4])];

	while (1) {
		static u32 i = 0;

		u64 address = swap_u64(dtb_mem_rsvmap[i + 0]);
		u64 size    = swap_u64(dtb_mem_rsvmap[i + 1]);

		if (address == 0 && size == 0) break;

		printf("|  0x%x bytes\n", address);
		printf("|  0x%x\n\n", size);

		i += 2;
	}
	// ----

	//u32 dtb_strings_len = swap_u32(header[8]);
	u8* dtb_strings     = &dtb[swap_u32(header[3])];

	u32  dtb_struct_len = swap_u32(header[9]) / 4;
	u32* dtb_struct     = (void*)&dtb[swap_u32(header[2])];

	u32 i = 0;
	while (i < dtb_struct_len) {

		static u8 indent = 0;
		static u8 tabs[16] = {'\0'};

		u32 token = swap_u32(dtb_struct[i]);

		switch (token) {
			case 0x01: {
				u64 len = strlen((u8*)&dtb_struct[i+1]);

				char* name = (indent > 0) ? (char*)&dtb_struct[i+1] : "Device Tree";

				printf("%s%s {\n", tabs, name);

				tabs[indent] = '\t';
				indent += 1;
				i += 1 + CEIL_DIV(len, 4);
				break;
			}
			case 0x02: {
				indent -= 1;
				tabs[indent] = '\0';
				printf("%s}\n", tabs);
				i += 1;
				break;
			}
			case 0x03: {
				u32 len    = swap_u32(dtb_struct[i+1]);
				u32 offset = swap_u32(dtb_struct[i+2]);

				printf("%s- %s: %s (%d)\n", tabs, &dtb_strings[offset], (u8*)&dtb_struct[i+3], len);

				i += 3 + CEIL_DIV(len, 4);
				break;
			}
			default: {
				i += 1;
				break;
			}
		}
	}
}
