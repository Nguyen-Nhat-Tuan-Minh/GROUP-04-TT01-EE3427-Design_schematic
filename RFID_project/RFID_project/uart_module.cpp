/*
 * uart_module.cpp
 *
 * Created: 01/03/2024 12:24:02 CH
 *  Author: Vũ Huy Bảo
 */ 

#include "uart_module.h"

void uart_init(){
	UCSR0A |= (1<<U2X0);
	UCSR0B = 1<<TXEN0;
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);
	UBRR0L = 103;
}


void uart_transmit_char(char a){
	while(!(UCSR0A&(1<<UDRE0)));
	UDR0 = a;
}



void uart_transmit_string(char* a){
	int i = 0;
	do {
		uart_transmit_char(*(a+i));
		i++;
	} while (*(a+i) != '\0');
}
