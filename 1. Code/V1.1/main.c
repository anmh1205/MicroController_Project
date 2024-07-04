#include <main.h>

#define ON 1
#define OFF 0

int8 SW_Pin[] = {Pin_B0, Pin_B1, Pin_B2, Pin_B3, Pin_B4};
int8 SW0 = Pin_B0;
int8 SW1 = Pin_B1;
int8 SW2 = Pin_B2;
int8 SW3 = Pin_B3;
int8 SW4 = Pin_B4;

int8 LED_Pin = Pin_D0;
int8 Buzzer_Pin = Pin_D1;

#define DOT_DURATION 250
#define DASH_DURATION (3 * DOT_DURATION)
#define MAX_MORSE_CODE_LENGTH 6

char morseCodeBuffer[MAX_MORSE_CODE_LENGTH + 1];
int morseCodeIndex = 0;

typedef struct
{
    char *morseCode;
    char character;
} MorseCode;

MorseCode morseCodeTable[] = {
    {".-", 'A'}, {"-...", 'B'}, {"-.-.", 'C'}, {"-..", 'D'}, {".", 'E'}, {"..-.", 'F'}, {"--.", 'G'},
    {"....", 'H'}, {"..", 'I'}, {".---", 'J'}, {"-.-", 'K'}, {".-..", 'L'}, {"--", 'M'}, {"-.", 'N'},
    {"---", 'O'}, {".--.", 'P'}, {"--.-", 'Q'}, {".-.", 'R'}, {"...", 'S'}, {"-", 'T'}, {"..-", 'U'},
    {"...-", 'V'}, {".--", 'W'}, {"-..-", 'X'}, {"-.--", 'Y'}, {"--..", 'Z'},
    {"-----", '0'}, {".----", '1'}, {"..---", '2'}, {"...--", '3'}, {"....-", '4'},
    {".....", '5'}, {"-....", '6'}, {"--...", '7'}, {"---..", '8'}, {"----.", '9'}};

void initTimer0()
{
    OPTION_REG = 0x07;
    TMR0 = 0;
    INTCON.T0IF = 0;
}

void startTimer()
{
    TMR0 = 0;
}

unsigned int stopAndReadTimer()
{
    unsigned int count = TMR0;
    return count;
}

void appendMorseCode(char symbol)
{
    if (morseCodeIndex < MAX_MORSE_CODE_LENGTH)
    {
        morseCodeBuffer[morseCodeIndex++] = symbol;
        morseCodeBuffer[morseCodeIndex] = '\0';
    }
}

void handleButtonPress()
{
    static int buttonState = 0;
    int currentState = input(PIN_B0);

    if (currentState == 1 && buttonState == 0)
    {
        startTimer();
    }
    else if (currentState == 0 && buttonState == 1)
    {
        unsigned int duration = stopAndReadTimer();
        if (duration < DOT_DURATION)
        {
            appendMorseCode('.');
        }
        else
        {
            appendMorseCode('-');
        }
    }

    buttonState = currentState;
}

void translateMorseCode()
{
    char character = morseCodeToCharacter(morseCodeBuffer);
    if (character != '\0')
    {
        lcd_putc(character);
    }
    clearMorseCodeBuffer();
}

char morseCodeToCharacter(char *code)
{
    for (int i = 0; i < sizeof(morseCodeTable) / sizeof(MorseCode); ++i)
    {
        if (strcmp(morseCodeTable[i].morseCode, code) == 0)
        {
            return morseCodeTable[i].character;
        }
    }
    return '\0';
}

void clearMorseCodeBuffer()
{
    memset(morseCodeBuffer, 0, sizeof(morseCodeBuffer));
    morseCodeIndex = 0;
}

void main()
{

    // Turning on the LCD
    initialize_lcd_i2c();

    // led and buzzer pin output
    set_tris_d(0x00);

    while (TRUE)
    {
        // printf(write_lcd_i2c, "First line.\nSecond line. \n");

        // output_d(0x02);
        // delay_ms(1000);

        // clear_lcd_i2c();

        // output_d(0x03);
        // delay_ms(1000);

        handleButtonPress();
    }
}
