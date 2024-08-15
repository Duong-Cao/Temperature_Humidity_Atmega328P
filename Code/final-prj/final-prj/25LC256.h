#ifndef AT25LC256_H
#define	AT25LC256_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* 25LC256_H */

//#include <xc.h>
#include "SPI.h"
#include <util/delay.h>

void EEPROM25LC256_write(unsigned int add, unsigned char data);

char EEPROM25LC256_read(unsigned int add);

