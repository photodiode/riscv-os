
#include <types.h>


u64 strlen(const char* s) {
	u64 len = 0;
	while (*s++ != '\0') len++;
	return len;
}


u32 strhash(const char* str) {
	if (!str) return 0;
	u32 hash = 5381; char c;
	while ((c = *str++)) hash = ((hash << 5) + hash) + c; // hash * 33 + c
	return hash;
}


u32 strnhash(const u32 len, const char* str) {
	if (!str || !len) return 0;
	u32 hash = 5381; char c;
	for (u32 i = 0; i < len; i++) {
		c = *str++;
		hash = ((hash << 5) + hash) + c; // hash * 33 + c
	}
	return hash;
}
