#include "LCD2004.h"

#define LCD_E PB0
#define LCD_RS PB1
#define LCD_D4 PC0
#define LCD_D5 PC1
#define LCD_D6 PC2
#define LCD_D7 PC3

void LCD_pin_init()
{
	// Set PB0 (E) and PB1 (RS) as output
	DDRB |= (1 << LCD_E) | (1 << LCD_RS);
	PORTB &= ~((1 << LCD_E) | (1 << LCD_RS)); // Set low

	// Set PC0-PC3 (D4-D7) as output
	DDRC |= (1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7);
	PORTC &= ~((1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7)); // Set low
}

void LCD_Init (void)    /* LCD Initialize function */
{
	LCD_pin_init();
	_delay_ms(20);        /* LCD Power ON Initialization time >15ms */
	LCD_Command (0x02);   /* Initialize LCD in 4-bit mode */
	LCD_Command (0x28);   /* 4-bit mode - 2 line - 5x7 font */
	LCD_Command (0x0F);   /* Display on, cursor blinking */
	LCD_Command (0x06);   /* Increment cursor */
	LCD_Command (0x01);   /* Clear display */
	LCD_Command (0x80);   /* Set cursor position to 0,0 */
}

void LCD_Command (char cmnd)    /* LCD command function */
{
	// Send upper nibble
	PORTC = (PORTC & 0xF0) | ((cmnd >> 4) & 0x0F);
	PORTB &= ~(1 << LCD_RS);    /* Command mode */
	PORTB |= (1 << LCD_E);
	_delay_us(1);
	PORTB &= ~(1 << LCD_E);
	_delay_us(200);

	// Send lower nibble
	PORTC = (PORTC & 0xF0) | (cmnd & 0x0F);
	PORTB |= (1 << LCD_E);    /* Enable pulse */
	_delay_us(1);
	PORTB &= ~(1 << LCD_E);
	_delay_ms(2);
}

void LCD_Data (char data)    /* LCD data write function */
{
	// Send upper nibble
	PORTC = (PORTC & 0xF0) | ((data >> 4) & 0x0F);
	PORTB |= (1 << LCD_RS);    /* Data mode */
	PORTB |= (1 << LCD_E);
	_delay_us(1);
	PORTB &= ~(1 << LCD_E);
	_delay_us(200);

	// Send lower nibble
	PORTC = (PORTC & 0xF0) | (data & 0x0F);
	PORTB |= (1 << LCD_E);    /* Enable pulse */
	_delay_us(1);
	PORTB &= ~(1 << LCD_E);
	_delay_ms(2);
}

void LCD_SetPosition(char row, char pos)
{
	if (row == 0)
	LCD_Command((pos & 0x1F) + 0x80);
	else if (row == 1)
	LCD_Command((pos & 0x1F) + 0xC0);   // 16x2: 16 columns, 0-15
	else if (row == 2)
	LCD_Command((pos & 0x1F) + 0x94);   // 20x4: 0-19
	else if (row == 3)
	LCD_Command((pos & 0x1F) + 0xD4);
}

void LCD_String (char *str)    /* Send string to LCD function */
{
	int i;
	for (i = 0; str[i] != 0; i++)  /* Send each char of string till NULL */
	{
		LCD_Data(str[i]);  /* Call LCD data write */
	}
}

void LCD_String_xy (unsigned char row, unsigned char pos, char *str)  /* Send string to LCD function */
{
	LCD_SetPosition(row, pos);
	LCD_String(str);   /* Call LCD string function */
}

void LCD_Custom_Char (unsigned char loc, unsigned char *msg)
{
	unsigned char i;
	if (loc < 8)
	{
		/* Command 0x40 and onwards forces the device to point CGRAM address */
		LCD_Command(0x40 + (loc * 8));
		for (i = 0; i < 8; i++)    /* Write 8 bytes for generation of 1 character */
		LCD_Data(msg[i]);
	}
}
