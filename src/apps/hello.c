
#include "../include/types.h"

#define asm __asm__ volatile


int  main();

void _start() {
	main();
	while (1);
}


void print(volatile char* str);
void wait();


int main() {
	char* str = "Hello  !\n";

	for (int i = 0; i < 5; i++) {
		str[6] = 65+i;
		print(str);
		wait();
	}

	return 0;
}


void print(volatile char* str) {
	asm("mv a1, %0" : : "r" ((u64)str));
	asm("li a0, 4");
	asm("ecall");
}

void wait() {
	for (volatile u64 i = 0; i < 0x3ffffff; i++);
}
