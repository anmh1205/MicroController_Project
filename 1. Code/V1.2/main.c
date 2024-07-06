#include <main.h>

// interrupt for uart receive

#INT_RDA
void UartReceive()
{
    char c = getc(PORT1);

    switch (c)
    {
    case 'c':
        clearAll();
        break;

    case 'b':
        playMorseCodeWithBuzzer();
        break;

    case 'l':
        playMorseCodeWithLED();
        break;

    default:
        morseCodeString[morseCodeStringIndex++] = c;
        set_lcd_i2c_cursor_position(1, morseCodeStringIndex - 1);
        printf(write_lcd_i2c, "%c", c);
        break;
    }
}

void switchMode()
{
    inputMode = !inputMode;

    clearAll();

    clear_lcd_i2c();

    if (inputMode == BUTTON_MODE)
    {
        set_lcd_i2c_cursor_position(1, 0);
        printf(write_lcd_i2c, "INPUT: BUTTON");

        disable_interrupts(INT_RDA);
        disable_interrupts(GLOBAL);
    }
    else
    {
        set_lcd_i2c_cursor_position(1, 0);
        printf(write_lcd_i2c, "INPUT: UART");

        enable_interrupts(INT_RDA);
        enable_interrupts(GLOBAL);
    }

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

    set_tris_d(0x00); // Set LED and buzzer pins as output
    set_tris_b(0x1F); // Set SW pins as input
    output_d(0x02);   // Turn off LED and Buzzer

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
    }
}
