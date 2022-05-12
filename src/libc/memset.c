
#include <types.h>

void* memset(void* str, const u8 c, const u64 size) {
	for (u64 i = 0; i < size; i++) {
		((u8*)str)[i] = c;
	}
	return str;
}
