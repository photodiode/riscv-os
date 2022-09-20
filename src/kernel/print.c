
#include <stdarg.h>

#include "uart.h"


int putchar(const char c) {
	return uart_write(c);
}


void puts(const char *s) {
	while (*s != '\0') {
		putchar(*s++);
	}
}


char* itostr(unsigned long i, const int base) {

	static char characters[]= "0123456789ABCDEF";
	static char buffer[65];
	char *ptr = &buffer[sizeof(buffer) - 1];
	*ptr = '\0';

	do {
		*--ptr = characters[i % base];
		i /= base;
	} while(i != 0);

	return ptr;
}


void printf(const char* format, ...) {

	va_list arg;
	va_start(arg, format);

	while (*format != '\0') {

		if (*format != '%') {
			putchar(*format);
			format++;
			continue;
		}

		format++;

		switch (*format) {
			case 'b': { // bit string
				const unsigned long i = va_arg(arg, unsigned long);
				puts("0b");
				puts(itostr(i, 2));
				break;
			}
			case 'c': { // character
				putchar((char)va_arg(arg, int));
				break;
			}
			case 'd': { //decimal integer
				long i = va_arg(arg, long);
				if (i < 0) {
					i = -i;
					putchar('-');
				}
				puts(itostr(i, 10));
				break;
			}
			case 'p': { // pointer address
				const unsigned long* i = va_arg(arg, unsigned long*);
				puts("0x");
				puts(itostr((unsigned long)i, 16));
				break;
			}
			case 's': { // string
				puts(va_arg(arg, char*));
				break;
			}
			case 'x': { // hex integer
				const unsigned long i = va_arg(arg, unsigned long);
				puts("0x");
				puts(itostr(i, 16));
				break;
			}
		}

		format++;
	}
}
