
#ifndef uart_h
#define uart_h

#define UART0 0x10000000UL

void uart_init();
char uart_read();
int  uart_write(char c);

#endif // uart_h
