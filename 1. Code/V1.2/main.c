#include <main.h>

void switchMode()
{
    inputMode = !inputMode;

    clearAll();

    clear_lcd_i2c();
    set_lcd_i2c_cursor_position(1, 0);
    printf(write_lcd_i2c, "Mode: %s", inputMode == BUTTON_MODE ? (char *)"Button" : (char *)"UART");

    delay_ms(1500);

    clear_lcd_i2c();
}

void handleButtonPress()
{
    if (input(SW_Pin[0]) == 0 && inputMode == BUTTON_MODE) // create morse code character
    {
        recordMorseCode();
    }

    else if (input(SW_Pin[1]) == 0 && inputMode == BUTTON_MODE) // append character to string and lcd
    {
        delay_ms(100); // Debounce

        completeMorseCode();

        while (input(SW_Pin[1]) == 0)
            ; // Wait for button release
    }

    else if (input(SW_Pin[2]) == 0) // play morse code with buzzer
    {
        delay_ms(50); // Debounce

        playMorseCodeWithBuzzer();

        while (input(SW_Pin[2]) == 0)
            ; // Wait for button release
    }

    else if (input(SW_Pin[3]) == 0) // play morse code with LED
    {
        delay_ms(50); // Debounce

        playMorseCodeWithLED();

        while (input(SW_Pin[3]) == 0)
            ; // Wait for button release
    }

    else if (input(SW_Pin[4]) == 0) // clear all
    {
        startTimer();

        delay_ms(50); // Debounce

        while (input(SW_Pin[4]) == 0 && getTimerValue() < 1000)
            ; // Wait for button release

        unsigned int16 duration = stopAndReadTimer();

        if (duration >= 1000)
        {
            switchMode();
        }
        else
        {
            clearAll();
        }
    }
}

void main()
{
    initialize_lcd_i2c(); // Initialize the LCD
    set_tris_d(0x00);     // Set LED and buzzer pins as output
    set_tris_b(0x1F);     // Set SW pins as input
    output_d(0x02);       // Turn off LED and Buzzer

    memset(morseCodeBufferString, 0, sizeof(morseCodeBufferString));
    memset(morseCodeString, 0, sizeof(morseCodeString));
    clear_lcd_i2c();

    set_lcd_i2c_cursor_position(1, 0);
    printf(write_lcd_i2c, "1. LE TRONG AN");
    set_lcd_i2c_cursor_position(2, 0);
    printf(write_lcd_i2c, "2. N.H VIET ANH");

    delay_ms(1500);
    clear_lcd_i2c();

    while (TRUE)
    {
        handleButtonPress();
        // Additional functionality can be added here
    }
}
