
#ifndef str_h
#define str_h

#include <types.h>

u64 strlen(const char* s);

u32 strhash(const char* str);
u32 strnhash(u32 len, const char* str);

#endif // str_h
