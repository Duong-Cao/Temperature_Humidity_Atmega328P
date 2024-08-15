

#ifndef SPI_H
#define	SPI_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* SPI_H */
//#include <xc.h>
#include <avr/io.h>


void SPI_MasterInit();
void SPI_MasterTransmit(unsigned char cData);
uint8_t SPI_MasterReceive();
void SPI_SlaveInit();
char SPI_SlaveReceive();


