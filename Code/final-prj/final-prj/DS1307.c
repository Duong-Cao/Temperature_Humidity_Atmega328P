/*
 * DS1307.c
 *
 * Created: 15/08/2022 9:48:39 SA
 *  Author: Administrator
 */ 
#include "DS1307.h"
uint8_t BCD2Dec(uint8_t BCD)
{
	return (BCD >> 4)*10 + (BCD & 0x0F);
}
uint8_t DecToBCD(uint8_t DEC)
{
	return (DEC/10) << 4 | (DEC % 10);
}

void DS1307_GetDateTime(DS1307 *rtc)
{
	I2C_Start();
	I2C_Write(WRITE_ADDR);
	I2C_Write(SEC_REGISTER);
	I2C_Stop();
	
	I2C_Start();
	I2C_Write(READ_ADDR);
	rtc->second = BCD2Dec(I2C_Read(ACK));
	rtc->minute = BCD2Dec(I2C_Read(ACK));
	rtc->hour = BCD2Dec(I2C_Read(ACK));
	rtc->day = BCD2Dec(I2C_Read(ACK));
	rtc->date = BCD2Dec(I2C_Read(ACK));
	rtc->month = BCD2Dec(I2C_Read(ACK));
	rtc->year = BCD2Dec(I2C_Read(NACK));
	I2C_Stop();
}

void DS1307_SetTime(uint8_t hour, uint8_t minute, uint8_t second)
{
	I2C_Start();
	I2C_Write(WRITE_ADDR);
	I2C_Write(SEC_REGISTER);
	second = DecToBCD(second);
	minute = DecToBCD(minute);
	hour = DecToBCD(hour);
	I2C_Write(second);
	I2C_Write(minute);
	I2C_Write(hour);
	I2C_Stop();
}

void DS1307_SetDate(uint8_t date, uint8_t month, uint8_t year)
{
	I2C_Start();
	I2C_Write(WRITE_ADDR);
	I2C_Write(DATE_REGISTER);
	date = DecToBCD(date);
	month = DecToBCD(month);
	year = DecToBCD(year);
	I2C_Write(date);
	I2C_Write(month);
	I2C_Write(year);
	I2C_Stop();
}