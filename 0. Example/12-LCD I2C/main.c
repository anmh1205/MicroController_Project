#include <main.h>

//-----------------------------------------------------------------------------

void main()
{
   unsigned int8 Dem=90;
   unsigned int16 A=256;
   char C =25;
   lcd_init();//khoi dong lcd
   lcd_clear();//xoa man hinh lcd
   
   lcd_gotoxy(1,1);
   printf(lcd_putc,"LCD-I2C!!!");
   
   while(TRUE)
   {
     lcd_gotoxy(1,2);
     printf(lcd_putc,"DEM:%d",Dem);
     
     lcd_gotoxy(10,2);
     printf(lcd_putc,"C:%d",C);
   }

}
//-----------------------------------------------------------------------------
