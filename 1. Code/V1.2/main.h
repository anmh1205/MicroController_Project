#include <16F887.h>
#device ADC = 16

#FUSES NOWDT      // No Watch Dog Timer
#FUSES PUT        // Power Up Timer
#FUSES NOBROWNOUT // No brownout reset
#FUSES NOLVP      // No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O

#use delay(crystal = 12000000)
#use rs232(baud = 9600, parity = N, xmit = PIN_C6, rcv = PIN_C7, bits = 8, stream = PORT1)
#use i2c(Master, Fast = 400000, sda = PIN_C4, scl = PIN_C3, force_sw)

#define PCF8574A       // Comment for modules using PCF8574
#define A2_A1_A0 0B111 // Address pin states (A2 A1 A0) of PCF8574A/PCF8574.
#define LCD16X2        // Comment this line to use 16x2 LCD

#include <string.h>
#include <lcd_i2c.c>


