/*
 * RFID_project.cpp
 *
 * Created: 14/02/2024 3:28:33 CH
 * Author : Vũ Huy Bảo
 */ 

#include "RFID.h"
#include "spi_module.h"
#include "uart_module.h"
#include "i2c_moudle.h"
#include "i2c_lcd.h"

	mfrc522 x(ss,0);
	
void main_init(){
	DDRC = (1<<0)|(1<<1)|(1<<2);
	i2c_init();
	i2c_lcd_init();
	spi_init();
	uart_init();
	x.RFID_init();
}


void print_lcd(uint8_t x){
	int temp;
	int hex[2];
	for (int i = 0; i<2; i++){
		temp = x % 16;
		x/=16;
		switch (temp){
			case 10:
			hex [i] = 'A';
			break;
						case 11:
						hex [i] = 'B';
						break;
									case 12:
									hex [i] = 'C';
									break;
												case 13:
												hex [i] = 'D';
												break;
															case 14:
															hex [i] = 'E';
															break;
																		case 15:
																		hex [i] = 'F';
																		break;
			default:
			hex[i] = temp + 0x30;
		}
	}
		for (int i = 1 ;i >= 0; i--){
			i2c_lcd_sendchar((char)(hex[i]));
		}
}

int main(void)
{	
	main_init();
	int status = mi_err;
	uint8_t serial[MAX_LEN];
	uint8_t buffer[4];
	
	buffer[0] = 0x46;
	buffer[1] = 0xE9;
	buffer[2] = 0xEC;
	buffer[3] = 0xAD;
	
	int cnt = 0,same = 0;
		
		i2c_lcd_position(1,1);
		i2c_lcd_sendstring("let start");
		_delay_ms(3000);
		i2c_lcd_sendcmd(lcd_cleardisplay);	
		uint8_t a = x.getfirmwareversion();
		if (a == 0x91) i2c_lcd_sendstring("version 1.0");
		else if (a == 0x92) i2c_lcd_sendstring("version 2.0");
		else i2c_lcd_sendstring("no receive cmd");
		

    /* Replace with your application code */
    while (1) 
    {
			same = 0;
			here:
			i2c_lcd_position(1,2);
			cnt++;
			if (cnt == 1)  i2c_lcd_sendstring("                "); // clear display just only 1 time
			status=x.requestag(serial);
			PORTC ^= (1<<2);
			_delay_ms(500);
			if(status == mi_ok) {
				status = x.anticollsion (serial);
				if (status == mi_ok) {
					cnt = 0;
					i2c_lcd_position(1,2);
					i2c_lcd_sendstring("NUID: ");
				//	uart_transmit_string("NUID: ");
					for (int i = 0;i < 4; i++){  
						print_lcd(serial[i]);
			//			uart_transmit_char(serial[i]);
					}
					
					
					for(int i = 0 ;i<4;i++){
						if (serial[i] == buffer[i]) same++;
					}
					
							if (same==4) {
								PORTC |= (1<<1);
								_delay_ms(5000);
								PORTC &= ~(1<<1);
							}
							else{
								PORTC |= (1<<0);
								_delay_ms(5000);
								PORTC &= ~(1<<0);
							}				
									
					

					status = x.selecttag(serial);
					status = x.halttag();
				}

			}
			}
	return 0;
}


