#include <main.h>

#define _LCD_FIRST_ROW          0x80     //Move cursor to the 1st row
#define _LCD_SECOND_ROW         0xC0     //Move cursor to the 2nd row
#define _LCD_THIRD_ROW          0x94     //Move cursor to the 3rd row
#define _LCD_FOURTH_ROW         0xD4     //Move cursor to the 4th row
#define _LCD_CLEAR              0x01     //Clear display
#define _LCD_RETURN_HOME        0x02     //Return cursor to home position, returns a 
                                         //shifted display to its original position.
                                         //Display data RAM is unaffected.
#define _LCD_CURSOR_OFF         0x0C     //Turn off cursor
#define _LCD_UNDERLINE_ON       0x0E     //Underline cursor on
#define _LCD_BLINK_CURSOR_ON    0x0F     //Blink cursor on
#define _LCD_MOVE_CURSOR_LEFT   0x10     //Move cursor left without changing 
                                         //display data RAM
#define _LCD_MOVE_CURSOR_RIGHT  0x14     //Move cursor right without changing 
                                         //display data RAM
#define _LCD_TURN_ON            0x0C     //Turn Lcd display on
#define _LCD_TURN_OFF           0x08     //Turn Lcd display off
#define _LCD_SHIFT_LEFT         0x18     //Shift display left without changing 
                                         //display data RAM
#define _LCD_SHIFT_RIGHT        0x1E     //Shift display right without changing 

                                         
#define LCD_RS PIN_B4
#define LCD_EN PIN_B5
#define LCD_D4 PIN_B0       
#define LCD_D5 PIN_B1  
#define LCD_D6 PIN_B2   
#define LCD_D7 PIN_B3

#define EN_DELAY 100
#define LCD_STROBE {output_high(LCD_EN); delay_us(EN_DELAY); output_low(LCD_EN); delay_us(EN_DELAY);}; 

#define  Waddr1 0x4E
#define  Waddr2 0x4C   
 
void I2C_LCD_Cmd(char address, char out_char);
void I2C_LCD_Chr(char address, char row, char column, char out_char);
void I2C_LCD_Chr_Cp(char address, char out_char);
void I2C_LCD_Init(char address);
void I2C_LCD_Out(char address, char row, char col, char *text);
void I2C_LCD_Out_Cp(char address, char *text);
void CustomChar(char address, char pos_row, char pos_char);
                                                           
char msg1[] = "I2C LCD 4 Bit";          
char msg2[] = "PIC18F46K22 8 MHz";                    
char msg3[] = "CCS C, EasyPIC v7";
char msg4[] = "LCD address 0x07";
char msg5[] = "LCD address 0x06";   
                     
char const msg6[] = "Const Text Test";      
          
char const character[] = {10,21,17,10,4,0,0,0};


/*   LCD_I2C found with PCF8574
         P7,P6,P5,P4 of PCF8574 = MSbits DB7,DB6,DB5,DB4 of LCD Display.
         P3 is hardware display bit (BackLight : 1 = on / 0 = off)
         P2 is hardware E display bit clock : E = 1 -> E = 0 active
         P1 is R/W hardware Read/Write bit hardware : Read = 1 / Write = 0
         P0 is RS hardware Register Select : CmdReg = 0 / DataReg = 1
*/

void I2C_LCD_Cmd(char address, char out_char) {

    char hi_n, lo_n;
    char rs = 0x00;

    hi_n = out_char & 0xF0;
    lo_n = (out_char << 4) & 0xF0;

    I2C_Start();
    I2C_Write(address);
    I2C_Write(hi_n | rs | 0x04 | 0x08);
    delay_us(50);
    I2C_Write(hi_n | rs | 0x00 | 0x08);
    delay_us(100);
    I2C_Write(lo_n | rs | 0x04 | 0x08);
    delay_us(50);
    I2C_Write(lo_n | rs | 0x00 | 0x08);
    I2C_stop();

    if(out_char == 0x01)Delay_ms(2);
}

void I2C_LCD_Chr(char address, char row, char column, char out_char) {

    char hi_n, lo_n;
    char rs = 0x01;

    switch(row){

        case 1:
        I2C_LCD_Cmd(address, 0x80 + (column - 1));
        break;
        case 2:
        I2C_LCD_Cmd(address, 0xC0 + (column - 1));                      
        break;
        case 3:
        #ifdef LCD16X4 
         I2C_LCD_Cmd(address, 0x90 + (column - 1));
        #else
         I2C_LCD_Cmd(address, 0x94 + (column - 1));
        #endif                                   
        break;                                              
        case 4:
        #ifdef LCD16X4 
         I2C_LCD_Cmd(address, 0xD0 + (column - 1)); 
        #else
         I2C_LCD_Cmd(address, 0xD4 + (column - 1));
        break;
    };                                                    

    hi_n = out_char & 0xF0;
    lo_n = (out_char << 4) & 0xF0;

    I2C_Start();
    I2C_Write(address);
    I2C_Write(hi_n | rs | 0x04 | 0x08);
    delay_us(50);
    I2C_Write(hi_n | rs | 0x00 | 0x08);
    delay_us(100);
    I2C_Write(lo_n | rs | 0x04 | 0x08);
    delay_us(50);
    I2C_Write(lo_n | rs | 0x00 | 0x08);
    I2C_stop();
}

void I2C_LCD_Chr_Cp(char address, char out_char) {  

    char hi_n, lo_n;
    char rs = 0x01;

    hi_n = out_char & 0xF0;
    lo_n = (out_char << 4) & 0xF0;

    I2C_Start();
    I2C_Write(address);
    I2C_Write(hi_n | rs | 0x04 | 0x08);
    delay_us(50);
    I2C_Write(hi_n | rs | 0x00 | 0x08);
    delay_us(100);
    I2C_Write(lo_n | rs | 0x04 | 0x08);
    delay_us(50);
    I2C_Write(lo_n | rs | 0x00 | 0x08);
    I2C_stop();
}


void I2C_LCD_Init(char address) {

     char rs = 0x00;
     
     I2C_Start();
     I2C_Write(address);  
     
     delay_ms(30);

     I2C_Write(0x30 | rs | 0x04 | 0x08);
     delay_us(50);
     I2C_Write(0x30 | rs | 0x00 | 0x08);
     
     delay_ms(10);

     I2C_Write(0x30 | rs | 0x04 | 0x08);
     delay_us(50);
     I2C_Write(0x30 | rs | 0x00 | 0x08);
     
     delay_ms(10);

     I2C_Write(0x30 | rs | 0x04 | 0x08);
     delay_us(50);
     I2C_Write(0x30 | rs | 0x00 | 0x08);
     
     delay_ms(10);

     I2C_Write(0x20 | rs | 0x04 | 0x08);
     delay_us(50);
     I2C_Write(0x20 | rs | 0x00 | 0x08);
     I2C_Stop();

     delay_ms(10);                                             

     I2C_LCD_Cmd(address, 0x28);
     I2C_LCD_Cmd(address, 0x06);
}                       

void I2C_LCD_Out(char address, char row, char col, char *text) {
    while(*text)
         I2C_LCD_Chr(address, row, col++, *text++);
}

void I2C_LCD_Out_Cp(char address, char *text) {    
    while(*text)
         I2C_LCD_Chr_Cp(address, *text++);         
} 

void CustomChar(char address,char pos_row, char pos_char) {
  char i;
    I2C_Lcd_Cmd(address, 64);
    for (i = 0; i<=7; i++) I2C_Lcd_Chr_Cp(address, character[i]);
    I2C_Lcd_Cmd(address, _LCD_RETURN_HOME);
    delay_ms(2);
    I2C_Lcd_Chr(address, pos_row, pos_char, 0);
} 
                                                            
void main() {                        
                                                 
   I2C_Speed(100000);
      
   I2C_LCD_Init(Waddr1); 
   I2C_LCD_Cmd(Waddr1,_LCD_CURSOR_OFF);                
   I2C_LCD_Cmd(Waddr1,_LCD_CLEAR);                          
   
   I2C_LCD_Init(Waddr2); 
   I2C_LCD_Cmd(Waddr2,_LCD_CURSOR_OFF);                
   I2C_LCD_Cmd(Waddr2,_LCD_CLEAR);
                   
   I2C_LCD_Out(Waddr1,1,1,msg1);     
   I2C_LCD_Out(Waddr1,2,1,msg2);                  
   I2C_LCD_Out(Waddr1,3,1,msg3);
   I2C_LCD_Out(Waddr1,4,1,msg4);              
                               
   I2C_LCD_Out(Waddr2,1,1,msg1);       
   I2C_LCD_Out(Waddr2,2,1,msg2);                  
   I2C_LCD_Out(Waddr2,3,1,msg3);
   I2C_LCD_Out(Waddr2,4,1,msg5);
   
   delay_ms(5000);               
                         
   I2C_LCD_Cmd(Waddr1, _LCD_CLEAR); 
    
   I2C_LCD_Out(Waddr1,1,1,(char*)msg6);
   I2C_LCD_Out(Waddr1,2,1,(char*)"Direct Text Passing");            
   CustomChar(Waddr1,3,10);
                   
   I2C_LCD_Cmd(Waddr2, _LCD_CLEAR);          
   
   I2C_LCD_Out(Waddr2,1,1,(char*)msg6);
   I2C_LCD_Out(Waddr2,2,1,(char*)"Direct Text Passing");            
   CustomChar(Waddr2,3,10);
               
   while(TRUE){                
      //TODO: User Code
   }                                   
}
