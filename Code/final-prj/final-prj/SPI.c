#include "SPI.h"

void SPI_MasterInit()
{
	/* Set MOSI and SCK output, all others input */
    DDRB |= (1<<DDB3)|(1<<DDB5)|(1<<DDB2);
    /* Enable SPI, Master, set clock rate fck/16 */
    /*SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);*/
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPI2X)|(1<<SPR0);
}

void SPI_MasterTransmit(unsigned char cData){
    /* Start transmission */
    SPDR = cData;
    /* Wait for transmission complete */
    while(!(SPSR & (1<<SPIF)));
}

uint8_t SPI_MasterReceive()
{
    // transmit dummy byte
    SPDR = 0xFF;

    // Wait for reception complete
    while(!(SPSR & (1 << SPIF)));

    // return Data Register
    return SPDR;
}

void SPI_SlaveInit(){
    /* Set MISO output, all others input */
    DDRB = (1<<DDB4) | (1 << DDB2);
    /* Enable SPI */
    SPCR = (1<<SPE);
}

char SPI_SlaveReceive(){
    /* Wait for reception complete */
    while(!(SPSR & (1<<SPIF)));
    /* Return Data Register */
    return SPDR;
}