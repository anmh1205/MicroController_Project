#include <main.h>

void main()
{

   set_tris_d(0x00);

   while(TRUE)
   {
      output_d(0x03);
      delay_ms(500);
      output_d(0x00);
      delay_ms(500);
   }
}
