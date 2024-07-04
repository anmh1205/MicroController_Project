#include <16F883.h>
#device ADC = 10

#FUSES PUT        // Power Up Timer
#FUSES MCLR       // Master Clear pin enabled
#FUSES NOPROTECT  // Code not protected from reading
#FUSES NOCPD      // No EE protection
#FUSES NOBROWNOUT // No brownout reset
#FUSES NOIESO     // Internal External Switch Over mode disabled
#FUSES NOFCMEN    // Fail-safe clock monitor disabled
#FUSES NOLVP      // No LOW V PROG, B3(PIC16) or B5(PIC18) used for I/O
#FUSES BORV40     // Brownout reset at 4.0V
#FUSES NOWRT      // Program memory not write protected
#use delay(internal = 8MHz)

#byte portb = 0x06
#byte trisb = 0x86

#bit led = portb .0
#bit trisb0 = trisb .0

/*********************************** I2C LCD MODULE **********************************************/
#use i2c(Master, Fast, I2C1, force_hw)
#define PCF8574A       // Comment for modules using PCF8574
#define A2_A1_A0 0B111 // Address pin states (A2 A1 A0) of PCF8574A/PCF8574.
#define LCD20X4        // Comment this line to use 16x2 LCD
#include <lcd_i2c.c>
/*************************************************************************************************/

void main()
{
    int b1, b2, b3, b4;
    int array[8];

    trisb0 = 0;
    led = 0;

    // Turning on the LCD
    initialize_lcd_i2c();

    printf(write_lcd_i2c, "\fFirst line.\nSecond line.");
    printf(write_lcd_i2c, "\nThird line.\nFourth line.");
    delay_ms(4000);

    // Scrolling to the right (without altering data)
    for (b1 = 0; b1 < 20; b1++)
    {
        scroll_lcd_i2c(RIGHT);
        delay_ms(500);
    }

    // Scrolling to the left (without altering data)
    for (b1 = 0; b1 < 10; b1++)
    {
        scroll_lcd_i2c(LEFT);
        delay_ms(500);
    }

    // Returning the LCD to normal position (cursor at column 1 and row 1)
    lcd_i2c_home_position();
    delay_ms(2000);

    // \a positions the cursor at the start of the first line
    printf(write_lcd_i2c, "\aABCDEFGHIJKLMNOPQRST"); // \a
    printf(write_lcd_i2c, "\nabcdefghijklmnopqrst");
    printf(write_lcd_i2c, "\n12345678901234567890");
    printf(write_lcd_i2c, "\n!@#$^&*(){}[]:;<>?/=");
    delay_ms(3000);

    // Clearing the LCD
    clear_lcd_i2c(); // Or printf(write_lcd_i2c, "\f");
    delay_ms(1000);

    // Placing a number in each corner and testing lcd_i2c_goto_xy() function
    lcd_i2c_goto_xy(4, 2);
    printf(write_lcd_i2c, " A number in");
    lcd_i2c_goto_xy(4, 3);
    printf(write_lcd_i2c, " each corner.");
    lcd_i2c_goto_xy(1, 1);
    printf(write_lcd_i2c, "1");
    lcd_i2c_goto_xy(20, 1);
    printf(write_lcd_i2c, "2");
    lcd_i2c_goto_xy(20, 4);
    printf(write_lcd_i2c, "3");
    lcd_i2c_goto_xy(1, 4);
    printf(write_lcd_i2c, "4");
    delay_ms(4000);

    // Reading characters from the LCD
    b1 = read_lcd_i2c_character(1, 1);  // Read character from column 1 row 1
    b2 = read_lcd_i2c_character(20, 1); // Read character from column 20 row 1
    b3 = read_lcd_i2c_character(20, 4);
    b4 = read_lcd_i2c_character(1, 4);
    printf(write_lcd_i2c, "\f  Characters read\n");
    printf(write_lcd_i2c, " from four corners:\n\n");
    printf(write_lcd_i2c, "      %c %c %c %c", b1, b2, b3, b4);
    delay_ms(4000);

    printf(write_lcd_i2c, "\f  Testing cursor.\nCursor on\nBlink off: ");
    lcd_i2c_cursor(CURSOR_ON | BLINK_OFF);
    delay_ms(4000);

    printf(write_lcd_i2c, "\f  Testing cursor.\nCursor off\nBlink on: ");
    lcd_i2c_cursor(CURSOR_OFF | BLINK_ON);
    delay_ms(4000);

    printf(write_lcd_i2c, "\f  Testing cursor.\nCursor on\nBlink on: ");
    lcd_i2c_cursor(CURSOR_ON | BLINK_ON);
    delay_ms(4000);

    // \r positions the cursor at the start of the line
    printf(write_lcd_i2c, "\fTest CR>>"); // CR - Carriage return '\r'
    delay_ms(1000);
    printf(write_lcd_i2c, "\r");
    delay_ms(3000);

    printf(write_lcd_i2c, "\fReading cursor\naddress.");
    lcd_i2c_goto_xy(11, 4);
    delay_ms(3000);
    b1 = read_cursor_address();
    printf(write_lcd_i2c, "\fCursor was at\nposition: 0x%X", b1);
    delay_ms(4000);
    lcd_i2c_cursor(CURSOR_OFF | BLINK_OFF);

    printf(write_lcd_i2c, "\f     Backlight\n     off");
    lcd_i2c_backlight(BL_OFF);
    delay_ms(3000);

    printf(write_lcd_i2c, "\f     Backlight\n     on");
    lcd_i2c_backlight(BL_ON);
    delay_ms(3000);

    printf(write_lcd_i2c, "\f     Backlight\n     off");
    lcd_i2c_backlight(BL_OFF);
    delay_ms(3000);

    // Testing CGRAM
    array[0] = 0b01010;
    array[1] = 0b01010;
    array[2] = 0b01010;
    array[3] = 0b00000;
    array[4] = 0b10001;
    array[5] = 0b01110;
    array[6] = 0b00000;
    array[7] = 0b00000;
    write_lcd_i2c_cgram(1, array); // Writes =) in CGRAM
                                   // position 1 (can be 0 to 7).
    array[0] = 0b01010;
    array[1] = 0b01010;
    array[2] = 0b01010;
    array[3] = 0b00000;
    array[4] = 0b01110;
    array[5] = 0b10001;
    array[6] = 0b00000;
    array[7] = 0b00000;
    write_lcd_i2c_cgram(7, array); // Writes =( in CGRAM
                                   // position 7 (can be 0 to 7).
    array[0] = 0b01010;
    array[1] = 0b11111;
    array[2] = 0b11111;
    array[3] = 0b01110;
    array[4] = 0b00100;
    array[5] = 0b00000;
    array[6] = 0b11111;
    array[7] = 0b00000;
    write_lcd_i2c_cgram(4, array); // Writes <3 in CGRAM
                                   // position 4 (can be 0 to 7).
    array[0] = 0b11111;
    array[1] = 0b10000;
    array[2] = 0b11111;
    array[3] = 0b00001;
    array[4] = 0b11111;
    array[5] = 0b10000;
    array[6] = 0b11111;
    array[7] = 0b00000;
    write_lcd_i2c_cgram(0, array);                          // Writes ?? in CGRAM
                                                            // position 0 (can be 0 to 7).
    printf(write_lcd_i2c, "\fDrawing Character\n\n      "); // Clear LCD
    draw_lcd_character(1);                                  // Write the drawn character at pos. 1
    printf(write_lcd_i2c, " ");
    draw_lcd_character(7); // Write the drawn character at pos. 7
    printf(write_lcd_i2c, " ");
    draw_lcd_character(4); // Write the drawn character at pos. 4
    printf(write_lcd_i2c, " ");
    draw_lcd_character(0); // Write the drawn character at pos. 0
    delay_ms(5000);

    printf(write_lcd_i2c, "\f\n     \1 END!! \1");

    while (TRUE)
    {
        led = !led;
        delay_ms(1000);
    }
}
