#ifndef VAR_H
#define VAR_H

#include <lcd_i2c.c>

const unsigned int16 DOT_DURATION = 200;
const unsigned int16 DASH_DURATION = 600;

const int8 SW_Pin[] = {Pin_B0, Pin_B1, Pin_B2, Pin_B3, Pin_B4};
// int8 SW_Pin = Pin_B0; // Assuming only one button for Morse code input

const int8 LED_Pin = Pin_D0;
const int8 Buzzer_Pin = Pin_D1;

unsigned int16 elapsedTime = 0;

char morseCodeBufferString[6];
unsigned int8 morseCodeBufferIndex = 0;

char morseCodeString[16];
unsigned int8 morseCodeStringIndex = 0;

typedef struct
{
    char morseCode[6];
    char character;
} MorseCode;

const MorseCode morseCodeTable[] = {
    {".-", 'A'}, //
    {"-...", 'B'},
    {"-.-.", 'C'},
    {"-..", 'D'},
    {".", 'E'},
    {"..-.", 'F'},
    {"--.", 'G'},
    {"....", 'H'},
    {"..", 'I'},
    {".---", 'J'},
    {"-.-", 'K'},
    {".-..", 'L'},
    {"--", 'M'},
    {"-.", 'N'},
    {"---", 'O'},
    {".--.", 'P'},
    {"--.-", 'Q'},
    {".-.", 'R'},
    {"...", 'S'},
    {"-", 'T'},
    {"..-", 'U'},
    {"...-", 'V'},
    {".--", 'W'},
    {"-..-", 'X'},
    {"-.--", 'Y'},
    {"--..", 'Z'},
    {"-----", '0'},
    {".----", '1'},
    {"..---", '2'},
    {"...--", '3'},
    {"....-", '4'},
    {".....", '5'},
    {"-....", '6'},
    {"--...", '7'},
    {"---..", '8'},
    {"----.", '9'}};

typedef enum
{
    BUTTON_MODE,
    UART_MODE
} Mode;

Mode tempMode = BUTTON_MODE;

#INT_TIMER1
void Interrupt()
{
    elapsedTime++;
    set_timer1(65161);
    clear_interrupt(INT_TIMER1);
}

void startTimer()
{
    elapsedTime = 0;

    // setup timer1, 1ms interrupt, 12MHz, timer1 16 bit, prescaler 1:8
    // 12MHz / 4 = 3MHz
    // 3Mhz / 8 = 375KHz
    // 1 / 375KHz = 2.6666666666666667us
    // 1ms / 2.6666666666666667us = 375
    // 65536 - 375 = 65161

    setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
    set_timer1(65161);
    enable_interrupts(INT_TIMER1);
    enable_interrupts(GLOBAL);
}

unsigned int16 getTimerValue()
{
    return elapsedTime;
}

unsigned int16 stopAndReadTimer()
{
    disable_interrupts(INT_TIMER1);
    disable_interrupts(GLOBAL);
    return elapsedTime;
}

void stopTimer()
{
    disable_interrupts(INT_TIMER1);
    disable_interrupts(GLOBAL);
}

#endif
