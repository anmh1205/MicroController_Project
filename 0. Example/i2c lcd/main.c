#include <xc.h>
#include "i2c.h"
#include "i2c_lcd.h"

#pragma config FOSC = INTRC_NOCLKOUT, WDTE = OFF, PWRTE = OFF
#pragma config MCLRE = OFF, CP = ON, CPD = ON, BOREN = ON
#pragma config IESO = ON, FCMEN = ON, LVP = OFF, DEBUG = OFF
#pragma config BOR4V = BOR21V, WRT = OFF

#define _XTAL_FREQ 4000000

void main(void)
{
	uint8_t tmp;

    TRISDbits.TRISD2 = 0;
    PORTDbits.RD2 = 0;
	
    I2C_LCD_Init();

    I2C_LCD_Clear();
	I2C_LCD_BackLight(1);
	I2C_LCD_Puts("ngohungcuong");
	I2C_LCD_NewLine();
	I2C_LCD_Puts("vidieukhien.org");

    while (1) {
        
    }
}
