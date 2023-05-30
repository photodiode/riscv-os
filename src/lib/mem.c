
#include <types.h>


void* memcpy(void* dest, const void* src, const u64 size) {
	for (u64 i = 0; i < size; i++) {
		((u8*)dest)[i] = ((u8*)src)[i];
	}
	return dest;
}


void* memset(void* ptr, const u8 c, const u64 size) {
	for (u64 i = 0; i < size; i++) {
		((u8*)ptr)[i] = c;
	}
	return ptr;
}
