#ifndef NRF24l01_H
#define NRF24l01_H

#include "stdint.h"
#include "nrf24l01_defines.h"

void nrf_writereg(uint8_t addr, uint8_t data);
uint8_t nrf_readreg(uint8_t addr);
void nrf_writebuf(uint8_t addr, uint8_t *buf, uint8_t count);
void nrf_readbuf(uint8_t addr, uint8_t *buf, uint8_t count);
#endif
