/*
 * DS1307.h
 *
 * Created: 15/08/2022 9:48:50 SA
 *  Author: Administrator
 */ 
#ifndef _DS1307_H_
#define  _DS1307_H_

#include "I2C_PROTOCOL.h"

#define WRITE_ADDR 0xD0
#define READ_ADDR 0xD1
#define SEC_REGISTER 0x00
#define MIN_REGISTER 0x01
#define HOUR_REGISTER 0x02
#define DATE_REGISTER 0x04
#define MONTH_REGISTER 0x05
#define YEAR_REGISTER 0x06

typedef struct {
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t date;
	uint8_t day;
	uint8_t month;
	uint8_t year;
} DS1307;

void DS1307_GetDateTime(DS1307 *rtc);
void DS1307_SetTime(uint8_t hour, uint8_t minute, uint8_t second);
void DS1307_SetDate(uint8_t date, uint8_t month, uint8_t year);
#endif /*_DS1307_H_*/