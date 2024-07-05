#include <main.h>


void appendMorseCode(int1 bit)
{
    morseCodeBuffer = (morseCodeBuffer << 1) | bit;
}

void clearMorseCodeBuffer()
{
    morseCodeBuffer = 0b0;
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