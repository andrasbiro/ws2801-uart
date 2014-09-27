#include "spi.h"

#define CPU_KHZ (F_CPU/1000)
void spiInitMaster(uint16_t clockrate){
	uint8_t spr;
	SET_BIT(MOSIDDR, MOSINUM);
	CLR_BIT(MISODDR, MISONUM);
	SET_BIT(SCKDDR, SCKNUM);
	SET_BIT(SSDDR, SSNUM);
	
	//calculate prescaler - this will be optimized, if clockrate is a const
	if( CPU_KHZ/clockrate >= 64 ){
		CLR_BIT(SPSR, SPI2X);
		spr = 3;
	} else if( CPU_KHZ/clockrate >= 32 ) {
		CLR_BIT(SPSR, SPI2X);
		spr = 2;
	} else if( CPU_KHZ/clockrate >= 16 ) {
		SET_BIT(SPSR, SPI2X);
		spr = 2;
	} else if( CPU_KHZ/clockrate >= 8 ) {
		CLR_BIT(SPSR, SPI2X);
		spr = 1;
	} else if( CPU_KHZ/clockrate >= 4 ) {
		SET_BIT(SPSR, SPI2X);
		spr = 1;
	} else if( CPU_KHZ/clockrate >= 2 ) {
		CLR_BIT(SPSR, SPI2X);
		spr = 0;
	} else {
		SET_BIT(SPSR, SPI2X);
		spr = 0;
	}
	
	SPCR = 1<<SPE | 1<<MSTR | spr;
}

void spiWrite(uint8_t data){
	SPDR = data;
}

uint8_t spiRead(){
	while( !READ_BIT(SPSR, SPIF) ); //wait for write operation
	return SPDR;
}
