#ifndef UART_H
#define UART_H

#define CHR_START				0x02
#define STOP_BYTE 		    0x03

void uart_init(uint32_t brr);
void uart_tch(uint8_t chr);
uint8_t uart_rch(void);
void uart_tstr(uint8_t *str);
void uart_rstr(char *intxt);
void uart_int(unsigned int c);
void uart_hex(uint8_t hexdig);

#endif
