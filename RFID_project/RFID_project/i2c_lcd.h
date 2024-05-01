/*
 * i2c_lcd.h
 *
 * Created: 02/03/2024 9:40:26 SA
 *  Author: Vũ Huy Bảo
 */ 


#ifndef I2C_LCD_H_
#define I2C_LCD_H_

#include "i2c_moudle.h"



#define slave_address  0x4E // slave address of piggyback board (PCF8574T)  is 0x27 + write(0) 
#define lcd_functionset     0x28   // Function Set: 4-bit mode, 2 lines, 5x8 font
#define lcd_displayoff      0x08   // Display Off
#define lcd_cleardisplay    0x01   // Clear Display
#define lcd_entrymodeset    0x06   // Entry Mode Set: Increment cursor, no display shift
#define lcd_displayon       0x0C   // Display On
#define lcd_shiftleft       0x1C
#define lcd_shiftright      0x18

void i2c_lcd_sendcmd(char cmd);
void i2c_lcd_init();
void i2c_lcd_sendchar(char data);
void i2c_lcd_position(int col, int row);
void i2c_lcd_sendstring(char* str);



#endif /* I2C_LCD_H_ */