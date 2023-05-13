
#ifndef mem_h
#define mem_h

#include <types.h>

void* memcpy(void* dest, const void* src, const u64 size);
void* memset(void* str, const u8 c, const u64 size);

#endif // mem_h
