/*
 * uart_module.h
 *
 * Created: 01/03/2024 12:24:32 CH
 *  Author: Vũ Huy Bảo
 */ 


#ifndef UART_MODULE_H_
#define UART_MODULE_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>

void uart_init();
void uart_transmit_char(char a);
void uart_transmit_string(char* a);




#endif /* UART_MODULE_H_ */