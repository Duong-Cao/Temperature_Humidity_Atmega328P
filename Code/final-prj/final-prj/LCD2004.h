
#ifndef LCD2004_H
#define LCD2004_H
/*Mode 4 bit
	MCU pin		LCD pin
	E			pinB0
	RS			pinB1
	D4-D7		pinB4-pinB7
*/

#include <avr/io.h>
#include <util/delay.h>

#define LCD_Port PORTB
#define LCD_Pin_E	0
#define LCD_Pin_RS	1

void LCD_pin_init();//don't use
void LCD_Init (void);									
void LCD_Command (char cmnd);
void LCD_Data (char data);
void LCD_SetPosition(char row, char pos);
void LCD_String (char *str);
void LCD_String_xy (unsigned char row, unsigned char pos, char *str);
void LCD_Custom_Char (unsigned char loc, unsigned char *msg);


#endif