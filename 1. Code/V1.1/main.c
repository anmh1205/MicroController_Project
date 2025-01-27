#include <main.h>

#define DOT_DURATION 100
#define DASH_DURATION 500

int8 SW_Pin[] = {Pin_B0, Pin_B1, Pin_B2, Pin_B3, Pin_B4};
// int8 SW_Pin = Pin_B0; // Assuming only one button for Morse code input

int8 LED_Pin = Pin_D0;
int8 Buzzer_Pin = Pin_D1;

unsigned int16 elapsedTime = 0;

unsigned int8 morseCodeBuffer = 0b0;
unsigned int8 morseCodeBufferString[6];
unsigned int8 morseCodeBufferIndex = 0;

unsigned int8 morseCodeString[16];
unsigned int8 morseCodeStringIndex = 0;
typedef struct
{
    unsigned int8 morseCode;
    char character;
} MorseCodeMapping;



// 0 is dot, 1 is dash
const MorseCodeMapping morseCodeTable[] = {
    {0b10, 'A'}, {0b0111, 'B'}, {0b0101, 'C'}, {0b011, 'D'}, {0b0, 'E'}, {0b1011, 'F'}, {0b110, 'G'}, {0b1111, 'H'}, {0b11, 'I'}, {0b1000, 'J'}, {0b101, 'K'}, {0b1101, 'L'}, {0b00, 'M'}, {0b01, 'N'}, {0b000, 'O'}, {0b1001, 'P'}, {0b0010, 'Q'}, {0b100, 'R'}, {0b111, 'S'}, {0b1, 'T'}, {0b1100, 'U'}, {0b11100, 'V'}, {0b100, 'W'}, {0b10100, 'X'}, {0b10101, 'Y'}, {0b1100, 'Z'}, {0b11111, '0'}, {0b01111, '1'}, {0b00111, '2'}, {0b00011, '3'}, {0b00001, '4'}, {0b00000, '5'}, {0b10000, '6'}, {0b11000, '7'}, {0b11100, '8'}, {0b11110, '9'}};

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

void appendMorseCode(int1 bit)
{
    morseCodeBuffer = (morseCodeBuffer << 1) | bit;
}

void clearMorseCodeBuffer()
{
    morseCodeBuffer = 0b0;
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
    for (int i = 0; i < sizeof(morseCodeTable) / sizeof(MorseCodeMapping); ++i)
    {
        if (morseCodeBuffer == morseCodeTable[i].morseCode)
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
    if (input(SW_Pin[0]) == 0) // create morse code character
    {

        startTimer();

        delay_ms(50);         // Debounce
        output_high(LED_Pin); // Turn on buzzer

        while (input(SW_Pin[0]) == 0)
        {
            if (elapsedTime > DASH_DURATION)
            {
                output_low(LED_Pin); // Turn off buzzer
            }
        }

        output_low(LED_Pin); // Turn off buzzer

        unsigned int16 duration = stopAndReadTimer();

        appendMorseCode(duration > DASH_DURATION ? 0b1 : 0b0);
        morseCodeBufferString[morseCodeBufferIndex++] = duration > DASH_DURATION ? '-' : '.';

        set_lcd_i2c_cursor_position(2, 0);
        printf(write_lcd_i2c, morseCodeBufferString);
    }

    else if (input(SW_Pin[1]) == 0) // append character to string and lcd
    {
        delay_ms(50); // Debounce

        translateMorseCode();

        clear_lcd_i2c();
        set_lcd_i2c_cursor_position(1, 0);
        printf(write_lcd_i2c, morseCodeString);

        while (input(SW_Pin[1]) == 0)
        {
            // Wait for button release
        }
    }

    else if (input(SW_Pin[2]) == 0) // play morse code with buzzer
    {
        delay_ms(50); // Debounce

        for (int i = 0; i < morseCodeStringIndex; ++i)
        {
            char c = morseCodeString[i];
            for (int j = 0; j < sizeof(morseCodeTable) / sizeof(MorseCodeMapping); ++j)
            {
                if (c == morseCodeTable[j].character)
                {
                    unsigned int8 morseCode = morseCodeTable[j].morseCode;
                    for (int k = 0; k < 5; ++k)
                    {
                        output_low(Buzzer_Pin);
                        delay_ms(morseCode & 0b10000 ? DASH_DURATION : DOT_DURATION);
                        output_high(Buzzer_Pin);
                        delay_ms(200);
                        morseCode = morseCode << 1;
                    }
                    delay_ms(800);
                }
            }
        }

        while (input(SW_Pin[2]) == 0)
        {
            // Wait for button release
        }
    }

    else if (input(SW_Pin[3]) == 0) // clear buffer
    {
        delay_ms(30); // Debounce

        clear_lcd_i2c();
        clearMorseCodeBuffer();
        clearMorseCodeString();

        while (input(SW_Pin[3]) == 0)
        {
            // Wait for button release
        }
    }

    else if (input(SW_Pin[4]) == 0) // clear all
    {
        delay_ms(30); // Debounce

        clear_lcd_i2c();
        clearMorseCodeBuffer();
        clearMorseCodeString();

        while (input(SW_Pin[2]) == 0)
        {
            // Wait for button release
        }
    }
}

void main()
{
    initialize_lcd_i2c(); // Initialize the LCD

    set_tris_d(0x00); // Set LED and buzzer pins as output
    set_tris_b(0x1F); // Set SW pins as input

    output_d(0x02); // Turn off LED and Buzzer

    memset(morseCodeBufferString, 0, sizeof(morseCodeBufferString));
    memset(morseCodeString, 0, sizeof(morseCodeString));

    clear_lcd_i2c();

    set_lcd_i2c_cursor_position(1, 0);
    printf(write_lcd_i2c, "LE TRONG AN");

    set_lcd_i2c_cursor_position(2, 0);
    printf(write_lcd_i2c, "N.H VIET ANH");

    delay_ms(1500);
    clear_lcd_i2c();

    while (TRUE)
    {
        handleButtonPress();
        // Additional functionality can be added here
    }
}
