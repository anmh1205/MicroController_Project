#include <main.h>

const unsigned int16 DOT_DURATION = 200;
const unsigned int16 DASH_DURATION = 600;

const int8 SW_Pin[] = {Pin_B0, Pin_B1, Pin_B2, Pin_B3, Pin_B4};
// int8 SW_Pin = Pin_B0; // Assuming only one button for Morse code input

const int8 LED_Pin = Pin_D0;
const int8 Buzzer_Pin = Pin_D1;

unsigned int16 elapsedTime = 0;

unsigned int8 morseCodeBufferString[6];
unsigned int8 morseCodeBufferIndex = 0;

unsigned int8 morseCodeString[16];
unsigned int8 morseCodeStringIndex = 0;

typedef struct
{
    char morseCode[6];
    char character;
} MorseCode;

const MorseCode morseCodeTable[] = {
    {".-", 'A'}, {"-...", 'B'}, {"-.-.", 'C'}, {"-..", 'D'}, {".", 'E'}, {"..-.", 'F'}, {"--.", 'G'}, {"....", 'H'}, {"..", 'I'}, {".---", 'J'}, {"-.-", 'K'}, {".-..", 'L'}, {"--", 'M'}, {"-.", 'N'}, {"---", 'O'}, {".--.", 'P'}, {"--.-", 'Q'}, {".-.", 'R'}, {"...", 'S'}, {"-", 'T'}, {"..-", 'U'}, {"...-", 'V'}, {".--", 'W'}, {"-..-", 'X'}, {"-.--", 'Y'}, {"--..", 'Z'}, {"-----", '0'}, {".----", '1'}, {"..---", '2'}, {"...--", '3'}, {"....-", '4'}, {".....", '5'}, {"-....", '6'}, {"--...", '7'}, {"---..", '8'}, {"----.", '9'}};

#INT_TIMER1
void An389_Interrupt()
{
    elapsedTime++;
    set_timer1(65036);
    clear_interrupt(INT_TIMER1);
}

void startTimer()
{
    elapsedTime = 0;

    // setup timer1, 1ms interrupt, 16MHz, timer1 16 bit, prescaler 1:8
    // 16MHz / 4 = 4MHz
    // 4MHz / 8 = 500KHz
    // 1 / 500KHz = 2us
    // 1ms / 2us = 500
    // 65536 - 500 = 65036
    setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
    set_timer1(65036);
    enable_interrupts(INT_TIMER1);
    enable_interrupts(GLOBAL);
}

unsigned int16 stopAndReadTimer()
{
    disable_interrupts(INT_TIMER1);
    disable_interrupts(GLOBAL);
    return elapsedTime;
}

void appendMorseCode(char morseChar)
{
    morseCodeBufferString[morseCodeBufferIndex++] = morseChar;
    morseCodeBufferString[morseCodeBufferIndex] = '\0'; // Null-terminate the string
}

void clearMorseCodeBuffer()
{
    memset(morseCodeBufferString, 0, sizeof(morseCodeBufferString));
    morseCodeBufferIndex = 0;
}

void clearMorseCodeString()
{
    memset(morseCodeString, 0, sizeof(morseCodeString));
    morseCodeStringIndex = 0;
}

void translateMorseCode()
{
    for (int i = 0; i < sizeof(morseCodeTable) / sizeof(MorseCode); ++i)
    {
        char temp[6];
        strcpy(temp, morseCodeTable[i].morseCode);

        if (strcmp(morseCodeBufferString, temp) == 0)
        {
            morseCodeString[morseCodeStringIndex++] = morseCodeTable[i].character;
            clearMorseCodeBuffer();
            return;
        }
    }
    morseCodeString[morseCodeStringIndex++] = '?';
    clearMorseCodeBuffer();
}

void handleButtonPress()
{
    if (input(SW_Pin[0]) == 0)
    { // create morse code character
        startTimer();
        delay_ms(50);           // Debounce
        output_high(LED_Pin);   // Turn on LED
        output_low(Buzzer_Pin); // Turn on buzzer

        while (input(SW_Pin[0]) == 0)
        {
            if (elapsedTime > DASH_DURATION)
            {
                output_low(LED_Pin);     // Turn off led
                output_high(Buzzer_Pin); // Turn off buzzer
            }
        }

        output_low(LED_Pin);     // Turn off LED
        output_high(Buzzer_Pin); // Turn off buzzer

        unsigned int16 duration = stopAndReadTimer();
        appendMorseCode(duration > DASH_DURATION ? '-' : '.');

        set_lcd_i2c_cursor_position(2, 0);
        printf(write_lcd_i2c, "%s", morseCodeBufferString);
    }

    else if (input(SW_Pin[1]) == 0)
    {                 // append character to string and lcd
        delay_ms(100); // Debounce
        translateMorseCode();
        clear_lcd_i2c();
        set_lcd_i2c_cursor_position(1, 0);
        printf(write_lcd_i2c, "%s", morseCodeString);

        while (input(SW_Pin[1]) == 0)
        {
            // Wait for button release
        }
    }

    else if (input(SW_Pin[2]) == 0)
    {                 // play morse code with buzzer
        delay_ms(50); // Debounce

        for (int i = 0; i < morseCodeStringIndex; ++i)
        {
            char c = morseCodeString[i];
            for (int j = 0; j < sizeof(morseCodeTable) / sizeof(MorseCode); ++j)
            {
                if (c == morseCodeTable[j].character)
                {
                    set_lcd_i2c_cursor_position(2, 0);
                    printf(write_lcd_i2c, "%c: %s", c, morseCodeTable[j].morseCode);

                    unsigned int8 k = 0;
                    while (morseCodeTable[j].morseCode[k] != '\0')
                    {
                        output_low(Buzzer_Pin);
                        unsigned int16 duration = morseCodeTable[j].morseCode[k] == '-' ? DASH_DURATION : DOT_DURATION;
                        delay_ms(duration);
                        output_high(Buzzer_Pin);

                        delay_ms(400);

                        k++;
                    }

                    set_lcd_i2c_cursor_position(2, 0);
                    printf(write_lcd_i2c, "                ");

                    delay_ms(800);
                }
            }
        }

        while (input(SW_Pin[2]) == 0)
        {
            // Wait for button release
        }
    }

    else if (input(SW_Pin[3]) == 0) // play morse code with LED
    {                               
        delay_ms(50);               // Debounce

        for (int i = 0; i < morseCodeStringIndex; ++i)
        {
            char c = morseCodeString[i];
            for (int j = 0; j < sizeof(morseCodeTable) / sizeof(MorseCode); ++j)
            {
                if (c == morseCodeTable[j].character)
                {
                    set_lcd_i2c_cursor_position(2, 0);
                    printf(write_lcd_i2c, "%c: %s", c, morseCodeTable[j].morseCode);

                    unsigned int8 k = 0;
                    while (morseCodeTable[j].morseCode[k] != '\0')
                    {
                        output_high(LED_Pin);
                        unsigned int16 duration = morseCodeTable[j].morseCode[k] == '-' ? DASH_DURATION : DOT_DURATION;
                        delay_ms(duration);
                        output_low(LED_Pin);

                        delay_ms(400);

                        k++;
                    }

                    set_lcd_i2c_cursor_position(2, 0);
                    printf(write_lcd_i2c, "                ");

                    delay_ms(800);
                }
            }
        }

        while (input(SW_Pin[3]) == 0)
        {
            // Wait for button release
        }
    }

    else if (input(SW_Pin[4]) == 0)
    {                 // clear all
        delay_ms(50); // Debounce
        clear_lcd_i2c();
        clearMorseCodeBuffer();
        clearMorseCodeString();

        while (input(SW_Pin[4]) == 0)
        {
            // Wait for button release
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
