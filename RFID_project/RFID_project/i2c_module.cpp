/*
 * i2c_module.cpp
 *
 * Created: 01/03/2024 12:00:32 CH
 *  Author: Vũ Huy Bảo
 */ 

#include "i2c_moudle.h"

void i2c_init(){
	TWSR = 0x00; //prescaler bit = 00
	TWBR = 72; // frequency = 50khz
	TWCR = (1<<TWEN);
}

void i2c_transmit(uint8_t data){
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN); // this is the rule for bot TWEN
	while (!(TWCR)&(1<<TWINT));
}


unsigned char i2c_read(unsigned char final){
	if (final == 0) TWCR  = (1<<TWINT)|(1<<TWEN)|(1<<TWEA); // send ACK
	else TWCR = (1<<TWINT)|(1<<TWEN); // send NACK
	while(!(TWCR&(1<<TWINT))) ;
	return TWDR;
}


void i2c_start(){
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTA); // start condition
	while(!(TWCR&(1<<TWINT))) ;
}


void i2c_stop(){
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO); // stop condition
}





