#include <16F883.h>
#device ADC=10

#FUSES PUT              //Power Up Timer
#FUSES MCLR             //Master Clear pin enabled
#FUSES NOPROTECT        //Code not protected from reading
#FUSES NOCPD            //No EE protection
#FUSES NOBROWNOUT       //No brownout reset
#FUSES NOIESO           //Internal External Switch Over mode disabled
#FUSES NOFCMEN          //Fail-safe clock monitor disabled
#FUSES NOLVP            //No LOW V PROG, B3(PIC16) or B5(PIC18) used for I/O
#FUSES BORV40           //Brownout reset at 4.0V
#FUSES NOWRT            //Program memory not write protected
#use delay(internal=8MHz)

#byte portb=0x06
#byte trisb=0x86

#bit led=portb.0
#bit trisb0=trisb.0

/*********************************** MODULO LCD I2C **********************************************/
#use i2c(Master, Fast, I2C1, force_hw)  
#define PCF8574A        // Comentar para módulos que usam o PCF8574                   
#define A2_A1_A0 0B111  // Estados dos pinos de endereço (A2 A1 A0) do PCF8574A/PCF8574.
#define LCD20X4         // Comentar esta linha para usar LCD 16x2
#include <lcd_i2c_pcf8574.c>
/*************************************************************************************************/

void main() {
   int b1,b2,b3,b4;
   int array[8];
   
   trisb0 = 0;
   led = 0;
   
   // Ligando o LCD
   inicializa_lcd_i2c();
   
   printf(escrever_lcd_i2c,"\fPrimeira linha.\nsegunda linha.");
   printf(escrever_lcd_i2c,"\nTerceira linha.\nQuarta linha.");
   delay_ms(4000);
   
   // Rotacionando para a direita (sem alterar dados)
   for(b1 = 0; b1 < 20; b1++) {
      deslocar_lcd_i2c(DIREITA);
      delay_ms(500);
   }
   
   // Rotacionando para a esquerda (sem alterar dados)
   for(b1 = 0; b1 < 10; b1++) {
      deslocar_lcd_i2c(ESQUERDA);
      delay_ms(500);
   }
   
   // Retornar o LCD para a posição normal (cursor na coluna 1 e linha 1)
   lcd_i2c_posicao_original();
   delay_ms(2000);
   
   // \a posiciona o cursor no inicio da primeira linha
   printf(escrever_lcd_i2c, "\aABCDEFGHIJKLMNOPQRST"); // \a
   printf(escrever_lcd_i2c, "\nabcdefghijklmnopqrst");  
   printf(escrever_lcd_i2c, "\n12345678901234567890");  
   printf(escrever_lcd_i2c, "\n!@#$^&*(){}[]:;<>?/=");
   delay_ms(3000);
   
   //Limpando o LCD
   limpar_lcd_i2c();// Ou printf(escrever_lcd_i2c,"\f");
   delay_ms(1000);
   
   //Colocando um número em cada canto e testando função lcd_i2c_irpara_xy();
   lcd_i2c_irpara_xy(4, 2);       
   printf(escrever_lcd_i2c, " Um numero em");   
   lcd_i2c_irpara_xy(4, 3);       
   printf(escrever_lcd_i2c, " cada canto.");   
   lcd_i2c_irpara_xy(1, 1);       
   printf(escrever_lcd_i2c, "1");   
   lcd_i2c_irpara_xy(20, 1);       
   printf(escrever_lcd_i2c, "2");   
   lcd_i2c_irpara_xy(20, 4);       
   printf(escrever_lcd_i2c, "3");   
   lcd_i2c_irpara_xy(1, 4);       
   printf(escrever_lcd_i2c, "4");   
   delay_ms(4000);
   
   //Lendo caracteres do LCD
   b1 = ler_caractere_lcd_i2c(1,1);  // Ler caractere da coluna 1 linha 1   
   b2 = ler_caractere_lcd_i2c(20,1); // ler caractere da coluna 20 linha 1
   b3 = ler_caractere_lcd_i2c(20,4);   
   b4 = ler_caractere_lcd_i2c(1,4);   
   printf(escrever_lcd_i2c, "\f  Caracteres lidos\n");
   printf(escrever_lcd_i2c, " dos quatro cantos:\n\n");
   printf(escrever_lcd_i2c, "      %c %c %c %c", b1, b2, b3, b4);       
   delay_ms(4000);
   
   printf(escrever_lcd_i2c,"\f  Testando cursor.\nCursor ligado\nBlink desligado: ");
   lcd_i2c_cursor(CURSOR_LIGADO | BLINK_DESLIGADO);
   delay_ms(4000);

   printf(escrever_lcd_i2c,"\f  Testando cursor.\nCursor desligado\nBlink ligado:    ");
   lcd_i2c_cursor(CURSOR_DESLIGADO | BLINK_LIGADO);
   delay_ms(4000);
 
   printf(escrever_lcd_i2c,"\f  Testando cursor.\nCursor ligado\nBlink ligado:    ");
   lcd_i2c_cursor(CURSOR_LIGADO | BLINK_LIGADO);
   delay_ms(4000);
   
   // \r posiciona o cursor no inicio da linha
   printf(escrever_lcd_i2c,"\fTeste CR>>"); // CR - Carriage return '\r'
   delay_ms(1000);
   printf(escrever_lcd_i2c,"\r");
   delay_ms(3000);
   
   printf(escrever_lcd_i2c,"\fLendo endereco do\ncursor.");
   lcd_i2c_irpara_xy(11, 4);
   delay_ms(3000);
   b1 = ler_endereco_cursor();
   printf(escrever_lcd_i2c,"\fCursor estava na\nposicao: 0x%X",b1);
   delay_ms(4000);
   lcd_i2c_cursor(CURSOR_DESLIGADO | BLINK_DESLIGADO);
   
   printf(escrever_lcd_i2c,"\f     Backlight\n     desligado");
   lcd_i2c_backlight(BL_DESLIGADO);
   delay_ms(3000);
   
   printf(escrever_lcd_i2c,"\f     Backlight\n     ligado");
   lcd_i2c_backlight(BL_LIGADO);
   delay_ms(3000);
   
   printf(escrever_lcd_i2c,"\f     Backlight\n     desligado");
   lcd_i2c_backlight(BL_DESLIGADO);
   delay_ms(3000);
   
   //testando CGRAM  
   array[0] = 0b01010;
   array[1] = 0b01010;
   array[2] = 0b01010;
   array[3] = 0b00000;
   array[4] = 0b10001;
   array[5] = 0b01110;
   array[6] = 0b00000;
   array[7] = 0b00000; 
   escrever_cgram_lcd_i2c(1,array); // Escreve =) na CGRAM 
                                    // posição 1 (pode ser de 0 a 7).
   array[0] = 0b01010;
   array[1] = 0b01010;
   array[2] = 0b01010;
   array[3] = 0b00000;
   array[4] = 0b01110;
   array[5] = 0b10001;
   array[6] = 0b00000;
   array[7] = 0b00000; 
   escrever_cgram_lcd_i2c(7,array); // Escreve =( na CGRAM 
                                    // posição 7 (pode ser de 0 a 7).
   array[0] = 0b01010;
   array[1] = 0b11111;
   array[2] = 0b11111;
   array[3] = 0b01110;
   array[4] = 0b00100;
   array[5] = 0b00000;
   array[6] = 0b11111;
   array[7] = 0b00000; 
   escrever_cgram_lcd_i2c(4,array); // Escreve <3 na CGRAM 
                                    // posição 4 (pode ser de 0 a 7).
   array[0] = 0b11111;
   array[1] = 0b10000;
   array[2] = 0b11111;
   array[3] = 0b00001;
   array[4] = 0b11111;
   array[5] = 0b10000;
   array[6] = 0b11111;
   array[7] = 0b00000; 
   escrever_cgram_lcd_i2c(0,array); // Escreve ?? na CGRAM 
                                    // posição 0 (pode ser de 0 a 7).
   printf(escrever_lcd_i2c,"\fDesenhando Caractere\n\n      ");   // Limpa LCD
   escrever_caractere_desenhado(1); // Escreve o caractere desenhado na pos. 1
   printf(escrever_lcd_i2c," ");
   escrever_caractere_desenhado(7); // Escreve o caractere desenhado na pos. 7
   printf(escrever_lcd_i2c," ");
   escrever_caractere_desenhado(4); // Escreve o caractere desenhado na pos. 4
   printf(escrever_lcd_i2c," ");
   escrever_caractere_desenhado(0); // Escreve o caractere desenhado na pos. 0
   delay_ms(5000);
   
   printf(escrever_lcd_i2c,"\f\n     \1 FIM!! \1");
   
   while(TRUE) {
      led = !led;
      delay_ms(1000);    
   } 
}
