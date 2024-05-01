/*
 * spi_module.cpp
 *
 * Created: 01/03/2024 12:03:41 CH
 *  Author: Vũ Huy Bảo
 */ 

#include "spi_module.h"


void spi_init(){
	DDRB = (1<<mosi)|(1<<sck)|(1<<ss);
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	sbi(PORTB,ss);
}

uint8_t spi_transmit(uint8_t data){
	uint8_t temp;
	SPDR = data;
	while (!(SPSR & (1<<SPIF)));
	temp = SPDR;  // clear flag
	return temp;
}