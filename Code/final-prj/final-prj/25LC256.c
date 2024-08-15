#include "25LC256.h"

void EEPROM25LC256_write(unsigned int add, unsigned char data){
    SPI_MasterInit();
    PORTB |= (1 << PB2);
    PORTB &= !(1 << PB2);
    SPI_MasterTransmit(0x06);
    PORTB |= (1 << PB2);
    PORTB &= !(1 << PB2);
    SPI_MasterTransmit(0x02);
    SPI_MasterTransmit(add >> 8);
    SPI_MasterTransmit(add & 0x00ff);
    SPI_MasterTransmit(data);
    PORTB |= (1 << PB2);
    _delay_ms(5);
}

char EEPROM25LC256_read(unsigned int add){
    SPI_MasterInit();
    PORTB |= (1 << PB2);
    PORTB &= !(1 << PB2);
    SPI_MasterTransmit(0x03);
    SPI_MasterTransmit(add >> 8);
    SPI_MasterTransmit(add & 0x00FF);
    uint8_t data = SPI_MasterReceive();
    PORTB |= (1 << PB2);
    return data;
}
