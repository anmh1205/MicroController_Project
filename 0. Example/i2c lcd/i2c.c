#include "i2c.h"

void i2c_init(void)
{

    SSPCONbits.SSPM3 = 1;
    SSPADD = 0x09;
    SSPCONbits.SSPEN = 1;
}

void i2c_start(void)
{

    SSPCON2bits.SEN = 1;
    while (SSPCON2bits.SEN) {

    }
}

void i2c_stop(void)
{
    
    SSPCON2bits.PEN = 1;
    while (SSPCON2bits.PEN) {
        
    }
}

uint8_t i2c_write(uint8_t u8Data)
{

    PIR1bits.SSPIF = 0;
    SSPBUF = u8Data;
    while (PIR1bits.SSPIF == 0) {

    }

    return SSPCON2bits.ACKSTAT;
}

uint8_t i2c_read(uint8_t u8Ack)
{
    uint8_t tmp;

    SSPCON2bits.RCEN = 1;
    while (SSPCON2bits.RCEN) {

    }
    tmp = SSPBUF;
    SSPCON2bits.RCEN = 0;
    if (u8Ack) {
        SSPCON2bits.ACKDT = 1;
    } else {
        SSPCON2bits.ACKDT = 0;
    }
    
    SSPCON2bits.ACKEN = 1;
    while (SSPCON2bits.ACKEN) {

    }

    return tmp;
}

void i2c_repeat_start(void)
{
    
    SSPCON2bits.RSEN = 1;
    while (SSPCON2bits.RSEN) {

    }
}
