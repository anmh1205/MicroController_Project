#include <16F877.h>
// #include <lcd_i2c.c>
#device ADC = 8

#FUSES NOWDT      // No Watch Dog Timer
#FUSES HS         // High speed Osc (> 4mhz for PCM/PCH) (>10mhz for PCD)
// #FUSES NOPUT      // No Power Up Timer
#FUSES NOBROWNOUT // No brownout reset
#FUSES NOLVP      // No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O

#use delay(crystal = 12000000)



// #use rs232(baud = 9600, parity = N, xmit = PIN_C6, rcv = PIN_C7, bits = 8, stream = PORT1)
// #use i2c(Master, Fast, sda = PIN_C3, scl = PIN_C4)
