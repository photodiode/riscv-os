
#ifndef uart_h
#define uart_h

#define UART0 0x10000000UL

void uart_init(void);
char uart_read(void);
int  uart_write(char c);

#endif // uart_h
