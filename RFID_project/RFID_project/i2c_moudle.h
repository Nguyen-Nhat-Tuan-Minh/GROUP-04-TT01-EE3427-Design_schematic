/*
 * i2c_moudle.h
 *
 * Created: 02/03/2024 7:21:20 SA
 *  Author: Vũ Huy Bảo
 */ 


#ifndef I2C_MOUDLE_H_
#define I2C_MOUDLE_H_

#include <avr/io.h>
#include <util/delay.h>
#define F_CPU 8000000UL

void i2c_init();
void i2c_transmit(uint8_t data);
unsigned char i2c_read(unsigned char final);
void i2c_start();
void i2c_stop();




#endif /* I2C_MOUDLE_H_ */