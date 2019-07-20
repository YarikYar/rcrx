#ifndef SPI_H
#define SPI_H
#include "stdint.h"

//#define SEND_BYTE_BEFORE_RECV
//#define BYTE_BEFORE_RECV 0xFF

void spi_init(uint16_t mode, uint16_t prescaler, uint16_t direction);
void spi_send(uint8_t data);
uint8_t spi_recv(void);
uint8_t spi_send_recv(uint8_t data);




#endif
