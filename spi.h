#ifndef __SPI_H__
#define __SPI_H__
#include <inttypes.h>
#include <avr/io.h>

#define SET_BIT(port, bit)    ((port) |= _BV(bit))
#define CLR_BIT(port, bit)    ((port) &= ~_BV(bit))
#define READ_BIT(port, bit)   (((port) & _BV(bit)) != 0)

#define SPI_MAXIMUM_CLOCKRATE 0xffff
#define SPI_MINIMUM_CLOCKRATE 1

void spiInitMaster(uint16_t clockrate);
void spiWrite(uint8_t data);
uint8_t spiRead();

#endif
