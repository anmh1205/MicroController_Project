#include <main.h>

void (*inputMode)(void);

void handleUartMode()
{
    if (kbhit())
    {
        char c = fgetc();

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

        case '\r':
            set_lcd_i2c_cursor_position(1, 0);
            printf(write_lcd_i2c, "%s", morseCodeString);
            break;

        default:
            if (morseCodeStringIndex < sizeof(morseCodeString))
            {
                morseCodeString[morseCodeStringIndex++] = c;
            }
            break;
        }
    }
}

void handleButtonMode()
{
    if (input(SW_Pin[0]) == 0 && morseCodeStringIndex < sizeof(morseCodeString)) // create morse code character
    {
        recordMorseCode();
    }

    else if (input(SW_Pin[1]) == 0) // append character to string and lcd
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
}

void switchMode()
{
    clearAll();

    clear_lcd_i2c();

    tempMode = !tempMode;

    if (tempMode == BUTTON_MODE)
    {
        set_lcd_i2c_cursor_position(1, 0);
        printf(write_lcd_i2c, "INPUT: BUTTON");

        inputMode = handleButtonMode;
    }
    else
    {
        set_lcd_i2c_cursor_position(1, 0);
        printf(write_lcd_i2c, "INPUT: UART");

        inputMode = handleUartMode;
    }

    delay_ms(1500);
    clear_lcd_i2c();
}

void readModeSelection()
{
    if (input(SW_Pin[4]) == 0) // clear all
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

    inputMode = handleButtonMode;

    while (TRUE)
    {
        (*inputMode)();

        readModeSelection();
    }
}
