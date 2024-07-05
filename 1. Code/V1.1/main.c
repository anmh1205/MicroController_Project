#include <main.h>

int8 SW_Pin[] = {Pin_B0, Pin_B1, Pin_B2, Pin_B3, Pin_B4};
// int8 SW_Pin = Pin_B0; // Assuming only one button for Morse code input

int8 LED_Pin = Pin_D0;
int8 Buzzer_Pin = Pin_D1;

// #define DOT_DURATION 250
// #define DASH_DURATION (3 * DOT_DURATION)
// #define MAX_MORSE_CODE_LENGTH 6

unsigned int16 elapsedTime = 0;

unsigned int8 morseCodeBuffer = 0b0;
unsigned int8 morseCodeString[16] = "aaaaaaa";
unsigned int8 morseCodeIndex = 0;
typedef struct
{
    unsigned int8 morseCode;
    char character;
} MorseCodeMapping;

MorseCodeMapping morseCodeTable[] = {
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
}

void clearMorseCodeString()
{
    memset(morseCodeString, 0, sizeof(morseCodeString));
    morseCodeIndex = 0;
}

void translateMorseCode()
{
    for (int i = 0; i < sizeof(morseCodeTable) / sizeof(MorseCodeMapping); ++i)
    {
        if (morseCodeBuffer == morseCodeTable[i].morseCode)
        {
            printf(write_lcd_i2c, "%c", morseCodeTable[i].character);
            morseCodeString[morseCodeIndex++] = morseCodeTable[i].character;

            clearMorseCodeBuffer();
            return;
        }
    }

    printf(write_lcd_i2c, "?");
    morseCodeString[morseCodeIndex++] = '?';
    clearMorseCodeBuffer();
}

void handleButtonPress()
{
    if (input(SW_Pin[0]) == 0)  // create morse code character
    {
        startTimer();
        output_low(Buzzer_Pin); // Turn on buzzer

        while (input(SW_Pin[0]) == 0)
        {
            if (elapsedTime > 400)
            {
                output_high(Buzzer_Pin); // Turn off buzzer
                delay_ms(1000);
                break;
            }
        }

        output_high(Buzzer_Pin); // Turn off buzzer

        unsigned int16 duration = stopAndReadTimer();

        appendMorseCode(duration > 400 ? 0b1 : 0b0);

        set_lcd_i2c_cursor_position(2, 1);
        send_lcd_i2c_string(morseCodeString);
    }

    if (input(SW_Pin[1]) == 0) // append character to string and lcd
    {
        translateMorseCode();

        while (input(SW_Pin[1]) == 0)
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

    output_d(0x02); // Turn off LED and Buzzer

    memset(morseCodeString, 0, sizeof(morseCodeString));

    clear_lcd_i2c();

    set_lcd_i2c_cursor_position(1, 1);
    printf(write_lcd_i2c, "Press SW1 to create");

    set_lcd_i2c_cursor_position(2, 1);
    printf(write_lcd_i2c, "Press SW1 to start");

    delay_ms(1500);
    clear_lcd_i2c();

    while (TRUE)
    {
        handleButtonPress();
        // Additional functionality can be added here
    }
}
