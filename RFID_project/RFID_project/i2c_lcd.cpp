/*
 * i2c_lcd.cpp
 *
 * Created: 02/03/2024 9:39:17 SA
 *  Author: Vũ Huy Bảo
 */ 

#include "i2c_lcd.h"


void i2c_lcd_sendcmd(char cmd){
	unsigned char upper, lower;
	lower = (cmd<<4)&0xF0;
	upper = cmd & 0xF0; 
	
	i2c_start();
	i2c_transmit(slave_address);
	_delay_ms(2);
	i2c_transmit(upper|0x0C);
	_delay_ms(2);
	i2c_transmit(upper|0x08);
	_delay_ms(2);
	i2c_transmit(lower|0x0C);
	_delay_ms(2);
	i2c_transmit(lower|0x08);
	_delay_ms(2);
	i2c_stop();
}


void i2c_lcd_init() {
	// Initialization sequence
	i2c_start();
	i2c_transmit(slave_address);
	_delay_ms(2);

	i2c_transmit(0x30|0x0C);
	_delay_ms(2);
	i2c_transmit(0x30|0x08);
	_delay_ms(5);
	
	i2c_transmit(0x30|0x0C);
	_delay_ms(2);
	i2c_transmit(0x30|0x08);
	_delay_ms(5);
	
	i2c_transmit(0x30|0x0C);
	_delay_ms(2);
	i2c_transmit(0x30|0x08);
	_delay_ms(5);
	
	i2c_transmit(0x20|0x0C);
	_delay_ms(2);
	i2c_transmit(0x20|0x08);
	_delay_ms(2);	
	i2c_stop();

	// Send initialization commands
	i2c_lcd_sendcmd(lcd_functionset); // Function Set: 4-bit mode, 2 lines, 5x8 font
	i2c_lcd_sendcmd(lcd_cleardisplay); // Clear Display
	i2c_lcd_sendcmd(lcd_entrymodeset); // Entry Mode Set: Increment cursor, no display shift
	i2c_lcd_sendcmd(lcd_displayon); // display on
}



void i2c_lcd_sendchar(char data){
	unsigned char upper, lower;
	lower = (data<<4)&0xF0;
	upper = data & 0xF0;
	
	i2c_start();
	i2c_transmit(slave_address);
	_delay_ms(2);
	i2c_transmit(upper|0x0D);
	_delay_ms(2);
	i2c_transmit(upper|0x09);
	_delay_ms(2);
	i2c_transmit(lower|0x0D);
	_delay_ms(2);
	i2c_transmit(lower|0x09);
	_delay_ms(2);
	i2c_stop();
}


void i2c_lcd_sendstring(char* str){
	while (*str != '\0'){
		i2c_lcd_sendchar(*(str++));
	}
}


void i2c_lcd_position(int col, int row) {
	switch(row) {
		case 1:
		i2c_lcd_sendcmd(0x80 | (0x00 + col - 1));
		break;
		case 2:
		i2c_lcd_sendcmd(0x80 | (0x40 + col - 1));
		break;
		default:
		break;
	}
}



