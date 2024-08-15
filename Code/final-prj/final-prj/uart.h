/*
 * uart.h
 *
 * Created: 4/9/2024 11:09:38 PM
 *  Author: Duong
 */ 


#ifndef UART_H_
#define UART_H_


#define BAUD 4800
#define MYUBRR F_CPU/16UL/BAUD-1

void USART_Init(unsigned int ubrr) // unsigned int ubrr
{
	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char) ubrr ;
	/* Enable receiver and transmitter */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	/*	UCSR0A = 0x00;*/
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01); // 8 bit data, 1 stop bit
	/*sei();*/
	
	UCSR0A |= (1<<U2X0); //Double (2x) baudrate. (C2)
}

void USART_Transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void UART_write(char *string) {
	for(int i=0; i<255; i++) {
		if(string[i] != 0) {
			USART_Transmit(string[i]);
		}
		else
			break;
	}
}

unsigned char USART_Receive(void)
{
	/* Wait for data to be received */
	while (!(UCSR0A & (1<<RXC0)))
	;
	/* Get and return received data from buffer */
	return UDR0;
}

#endif /* UART_H_ */