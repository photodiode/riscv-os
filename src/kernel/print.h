
#ifndef print_h
#define print_h

#include <riscv.h>

int  putchar(char c);
void puts(char *s);
void printf(char* format, ...);

#define fatal(...) { printf("\33[31;1mFatal:\33[0m "); printf(__VA_ARGS__); asm("wfi"); }

#endif // print_h
