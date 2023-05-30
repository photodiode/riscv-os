
#include <stdarg.h>

#include <riscv.h>
#include <sbi.h>


static char* itostr(unsigned long i, const int base) {

	static const char characters[] = "0123456789abcdef";
	static char buffer[65] = {0};
	char *ptr = &buffer[sizeof(buffer) - 1];
	*ptr = '\0';

	do {
		*--ptr = characters[i % base];
		i /= base;
	} while(i != 0);

	return ptr;
}


void putchar(const char c) {
	sbi_console_putchar(c);

	return;
}


void puts(const char *s) {
	while (*s != '\0') {
		putchar(*s++);
	}
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
				puts(itostr(i, 16));
				break;
			}
		}

		format++;
	}
}
