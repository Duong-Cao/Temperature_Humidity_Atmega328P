/*
 * final-prj.c
 *
 * Created: 5/10/2024 11:13:29 AM
 * Author : Duong
 */
 
#define F_CPU 1000000UL
#define dht11_pin PD6

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "I2C_PROTOCOL.h"
#include "uart.h"
#include "DS1307.h"
#include "dht11.h"
#include "LCD2004.h"
#include "SPI.h"
#include "25LC256.h"



volatile int five_second = 0; // variable to estimate 5 seconds in UART
volatile int send_uart = 0; // variable to allow send data exceeded UART

unsigned char temp, humid; // Temperature and humidity

unsigned char temp_max = 48; // Threshold temp
unsigned char humid_max = 75; // Threshold humid
unsigned char read_eeprom_humid_max, read_eeprom_temp_max; // read EEPROMs
unsigned int dht11_bool; // DHT11
unsigned int mode_display = 0; // Display change mode, 0: Monitoring mode, 1: Setting up threshold mode
unsigned int mode_threshold = 0; // Change mode for temp or humid, 0: change threshold temp, 1: change threshold humid
DS1307 rtc;

char str[100]; // String to send UART

// Function prototypes
void displayLCD_data();
void check_mode_display_button();
void check_up_down_button();
void led_buzzer();
void ports_init();
void displayThreshold();
void timer1_init();
void write_data_eeprom();
void send_uart_data_exceed();
//


// Display LCD data function
void displayLCD_data() {
	LCD_String_xy(0,0,"Temperature:");
	LCD_String_xy(1,0,"Humidity:");
	LCD_String_xy(2,0,"Time:");
	LCD_String_xy(3,0,"Date:");
	
	// TIME
	LCD_SetPosition(2,6);
	LCD_Data(rtc.hour/10+48);
	LCD_Data(rtc.hour%10+48);
	LCD_Data(':');
	LCD_Data(rtc.minute/10 + 48);
	LCD_Data(rtc.minute%10 + 48);
	LCD_Data(':');
	LCD_Data(rtc.second/10 + 48);
	LCD_Data(rtc.second%10 + 48);
	LCD_SetPosition(3,6);
	LCD_Data(rtc.date/10 + 48);
	LCD_Data(rtc.date%10 + 48);
	LCD_Data('/');
	LCD_Data(rtc.month/10 + 48);
	LCD_Data(rtc.month%10 + 48);
	LCD_Data('/');
	LCD_Data(rtc.year/10 + 48);
	LCD_Data(rtc.year%10 + 48);
	
	// DHT11 LCD display
	if(dht11_bool) {
		LCD_SetPosition(0,13);
		LCD_Data(temp/10+48);
		LCD_Data(temp%10+48);
		LCD_SetPosition(1,10);
		LCD_Data(humid/10+48);
		LCD_Data(humid%10+48);
	}

}


// Check mode display button
void check_mode_display_button() {
	// Button MODE
	if( !(PIND & (1 << PD3)) ) {
		_delay_ms(20);
		if( !(PIND & (1 << PD3)) ) {
			mode_display = ~mode_display;
			LCD_Command(0x01);
		}
		while(!(PIND & (1 << PD3)));
	}
}


// Check up and down button (changing threshold)
void check_up_down_button(){
	
	// Button MODE_THREHOLD
	if( !(PIND & (1 << PD4)) ) {
		_delay_ms(20);
		if( !(PIND & (1 << PD4)) ) {
			mode_threshold = ~mode_threshold;			
		}
		while(!(PIND & (1 << PD4)));
	}
	
	// Button UP/DOWN
	if( !(PINB & (1 << PB6)) ) {
		_delay_ms(20);
		if( !(PINB & (1 << PB6)) ) {
			if(mode_threshold==0)
				temp_max+=1;
			else
				humid_max+=1;
		}
		while(!(PINB & (1 << PB6)));
	}

	if( !(PINB & (1 << PB7)) ) {
		_delay_ms(20);
		if( !(PINB & (1 << PB7)) ) {
			if(mode_threshold==0)
				temp_max-=1;
			else
				humid_max-=1;
		}
		while(!(PINB & (1 << PB7)));
	}
}


// Led, buzzer warning
void led_buzzer() {
	// if temp or humid >= temp max or huimid max
	if( (temp >= temp_max) || (humid >= humid_max) ) {
		// Led on/off
		PORTD ^= (1<<PD2); 
		_delay_ms(50);
		PORTD &= ~(1<<PD5); // buzzer ON
	}
	else if( (temp < temp_max) || (humid < humid_max) ) {
		PORTD &= ~(1<<PD2); // Led OFF
		PORTD |= (1<<PD5); // buzzer OFF
	}
}


// port init function
void ports_init(){
	DDRB &= ~((1<<PB6) | (1<<PB7)); // UP, DOWN button
	/*DDRB |= (1<<PB0);*/
	DDRD |= ((1<<PD2) | (1<<PD5)); // PD2 -> led, PD5 -> buzzer
	PORTD |= (1<<PD5); // pull-up resistor PD5
	DDRD &= ~((1<<PD3) | (1<<PD4)); // MODE_DISPLAY and MODE_THRESHOLD button
}


// Display LCD setting mode
void displayThreshold(){
	LCD_String_xy(0,0,"   THRESHOLD MODE");
	LCD_String_xy(2,0,"Temperature MAX:");
	LCD_String_xy(3,0,"Humidity MAX:");
	
	LCD_String_xy(1,0,"Setting:");
	LCD_SetPosition(1,9);
	if(mode_threshold==0){
		LCD_String("TEMPERATURE");
	}
	else{
		LCD_String("HUMIDITY   ");
	}
	
	LCD_SetPosition(2,17);
	LCD_Data(temp_max/10 + 48);
	LCD_Data(temp_max%10 + 48);
	LCD_SetPosition(3,17);
	LCD_Data(humid_max/10 + 48);
	LCD_Data(humid_max%10 + 48);
}


// Timer1 init
void timer1_init(){
	// set up timer with prescaler = 64 and CTC mode
	TCCR1B |= (1 << WGM12)|(1 << CS11)|(1<<CS10);
	// initialize counter
	TCNT1 = 0;
	// initialize compare value - 1 sec
	OCR1A = 15625;
	// enable compare interrupt
	TIMSK1 |= (1 << OCIE1A);
	// enable global interrupts
	sei();
	
}


// Write data to epprom function
void write_data_eeprom(){
	read_eeprom_temp_max = EEPROM25LC256_read(0);
	read_eeprom_humid_max = EEPROM25LC256_read(1);
	if(read_eeprom_temp_max != temp_max)	EEPROM25LC256_write(0,temp_max); // index 0: temp max
	if(read_eeprom_humid_max != humid_max)	EEPROM25LC256_write(1,humid_max); // index 1: humid max
	
	if(humid >= read_eeprom_humid_max || temp >= read_eeprom_temp_max){
		EEPROM25LC256_write(3, rtc.date); // index 3: date
		EEPROM25LC256_write(4, rtc.month); // index 4: month
		EEPROM25LC256_write(5, rtc.year); // index 5: year
		EEPROM25LC256_write(6, rtc.hour); // index 6: hour
		EEPROM25LC256_write(7, rtc.minute); // index 7: minute
		EEPROM25LC256_write(8, rtc.second); // index 8: second
		EEPROM25LC256_write(9, temp); // index 9: current temp
		EEPROM25LC256_write(10, humid); // index 10: current humid
	}
}


// UART send function
void send_uart_data_exceed(){
		// if only temp exceed --> send tem[
		if(temp >= temp_max && humid < humid_max){
			sprintf(str, "Temperature exceeded: ");
			UART_write(str);
			USART_Transmit(temp/10+48);
			USART_Transmit(temp%10+48);
		}
		// if only humid exceed --> send humid
		else if(humid >= humid_max && temp < temp_max){
			sprintf(str, "Humidity exceeded: ");
			UART_write(str);
			USART_Transmit(humid/10+48);
			USART_Transmit(humid%10+48);
		}
		// if both humid and temp exceed --> send humid and temp
		else if(temp >= temp_max && humid >= humid_max){
			sprintf(str, "Humidity and Temperature exceeded: ");
			UART_write(str);
			USART_Transmit(humid/10+48);
			USART_Transmit(humid%10+48);
			USART_Transmit(' ');
			USART_Transmit(temp/10+48);
			USART_Transmit(temp%10+48);
		}
		
		if(temp >= temp_max || humid >= humid_max){
			// Send time exceed
			USART_Transmit('\r');
			USART_Transmit(rtc.hour/10+48);
			USART_Transmit(rtc.hour%10+48);
			USART_Transmit(':');
			USART_Transmit(rtc.minute/10 + 48);
			USART_Transmit(rtc.minute%10 + 48);
			USART_Transmit(':');
			USART_Transmit(rtc.second/10 + 48);
			USART_Transmit(rtc.second%10 + 48);
			USART_Transmit('\r');
			sprintf(str, "----------------\r");
			UART_write(str);
		}
}


int main(void)
{
	ports_init();
	LCD_Init();
	LCD_Command(0x0C);  // Display ON, Cursor OFF, Blink OFF
	USART_Init(MYUBRR);
	
	timer1_init();
	
	while(1)
	{
		DS1307_GetDateTime(&rtc);
		dht11_bool = dth11_read(&temp, &humid);
		
		if(!mode_display){
			displayLCD_data();
		}
		else{
			displayThreshold();
		}
		
		if(send_uart){
			send_uart_data_exceed();
			send_uart = 0;
		}
		
		led_buzzer();
		
		check_mode_display_button();
		check_up_down_button();
		write_data_eeprom();
				
	}
}


// timer1 compa vector
ISR (TIMER1_COMPA_vect){
	five_second += 1;
	if(five_second == 5){
		send_uart = 1;
		five_second = 0;
	}
}