/*
 * I2C_PROTOCOL.c
 *
 * Created: 15/08/2022 9:48:17 SA
 *  Author: Administrator
 */ 
#include "I2C_PROTOCOL.h"

void I2C_Init()
{
	TWBR = ((F_CPU/F_SCL)-16)/2;
}

void I2C_Start()
{
	TWCR  = _SB(TWINT) | _SB(TWSTA) | _SB(TWEN);
	while(!(TWCR & _SB(TWINT)));
}

void I2C_Stop()
{
	TWCR  = _SB(TWINT) | _SB(TWSTO) | _SB(TWEN);
	while(TWCR & 1<<TWSTO);
}

void I2C_Write(uint8_t dataSend)
{
	TWDR = dataSend;
	TWCR  = _SB(TWINT) | _SB(TWEN);
	while(!(TWCR & _SB(TWINT)));
}

uint8_t I2C_Read(int sig)
{
	TWCR = _SB(TWINT) | sig <<TWEA | _SB(TWEN);
	while(!(TWCR & _SB(TWINT)));
	return TWDR;
}

