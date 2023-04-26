
#include "../include/types.h"

#define asm __asm__ volatile


int main(void);

void _start(void) {
	main();
	while (1);
}



void print(volatile char* str);
void putchar(char c);
void wait(void);


int main(void) {
	for (int i = 0; i < 5; i++) {
		putchar(65+i);
		wait();
	}

	return 0;
}

void putchar(char c) {
	asm("mv a1, %0" : : "r" (c));
	asm("li a0, 3");
	asm("ecall");
}

void wait(void) {
	for (volatile u64 i = 0; i < 0x1ffffff; i++);
}
