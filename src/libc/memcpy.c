
#include <types.h>

void* memcpy(void* dest, const void* src, const u64 size) {
	for (u64 i = 0; i < size; i++) {
		((u8*)dest)[0] = ((u8*)src)[0];
	}
	return dest;
}
