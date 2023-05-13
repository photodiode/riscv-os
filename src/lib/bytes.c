
#include <types.h>


u16 swap_u16(u16 v) {
	return (
		((v & (0xffU<<0)) << 8) |
		((v & (0xffU<<8)) >> 8)
	);
}

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
