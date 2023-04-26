
#include <types.h>

#include "uart.h"

#define RBR 0x00
#define THR 0x00
#define IER 0x01
#define FCR 0x02
#define LCR 0x03
#define LSR 0x05


static volatile u8* const uart = (void*)UART0;

void uart_init(void) {

	uart[LCR] |= (3 & 0x03) << 0; // set word length select bits to 2^3 (8)
	uart[FCR] |= (1 << 0);        // enable fifo
	uart[IER] |= (1 << 0);        // enable receiver buffer interrupts

	uart[LCR] |= (1 << 7);        // set blab
	*((u16*)uart) = 592;          // ceil( (clock_hz) / (baud_sps x 16) )
	uart[LCR] &= ~(1 << 7);       // clear blab
}

char uart_read(void) {
	if ((uart[LSR] & 0x01) == 1) {
		return uart[RBR];
	}
	return 0;
}

int uart_write(const char c) {
	while ((uart[LSR] & 0x40) == 0);
	return uart[THR] = c;
}
