/*
 * spi_module.h
 *
 * Created: 01/03/2024 12:02:16 CH
 *  Author: Vũ Huy Bảo
 */ 


#ifndef SPI_MODULE_H_
#define SPI_MODULE_H_


#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>


#define F_CPU 8000000UL
#define mosi 3
#define sck 5
#define ss 2

#define sbi(port,bit) {port|= (1<<bit);}
#define cbi(port,bit) {port&= (~(1<<bit));}
#define none_value 0x00


void spi_init();
uint8_t spi_transmit(uint8_t data);



#endif /* SPI_MODULE_H_ */