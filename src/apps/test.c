
#include "../include/types.h"

#define asm __asm__ volatile


static inline u64 get_tp() {
	u64 x;
	asm("mv %0, tp" : "=r" (x));
	return x;
}

#define HART_ID get_tp()


void _start() {

	volatile char* str = "Hello, -!\n";
	str[7] = 65 + HART_ID;

	asm("mv a1, %0" : : "r" ((u64)str));
	asm("li a0, 4");
	asm("ecall");

	while (1);
}
