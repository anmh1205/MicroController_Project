#include <globalVariables.h>

void playMorseCodeWithBuzzer()
{
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
                    unsigned int16 duration = morseCodeTable[j].morseCode[k] == '-' ? DASH_DURATION : DOT_DURATION;

                    startTimer();
                    output_low(Buzzer_Pin);

                    while (elapsedTime < duration)
                        ;

                    output_high(Buzzer_Pin);
                    stopTimer();

                    delay_ms(400);

                    k++;
                }

                set_lcd_i2c_cursor_position(2, 0);
                printf(write_lcd_i2c, "                ");

                delay_ms(800);
            }
        }
    }
}

void playMorseCodeWithLED()
{
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
                    unsigned int16 duration = morseCodeTable[j].morseCode[k] == '-' ? DASH_DURATION : DOT_DURATION;

                    startTimer();
                    output_high(LED_Pin);

                    while (elapsedTime < duration)
                        ;

                    output_low(LED_Pin);
                    stopTimer();

                    delay_ms(400);

                    k++;
                }

                set_lcd_i2c_cursor_position(2, 0);
                printf(write_lcd_i2c, "                ");

                delay_ms(800);
            }
        }
    }
}