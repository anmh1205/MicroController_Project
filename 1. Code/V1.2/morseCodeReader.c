#include <globalVariables.h>
// #include <lcd_i2c.c>

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

void clearAll()
{
   clearMorseCodeBuffer();
   clearMorseCodeString();

   clear_lcd_i2c();
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

void recordMorseCode()
{
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

void completeMorseCode()
{
   if (morseCodeBufferIndex > 0)
   {
      translateMorseCode();
      clear_lcd_i2c();
      set_lcd_i2c_cursor_position(1, 0);
      printf(write_lcd_i2c, "%s", morseCodeString);
   }
}