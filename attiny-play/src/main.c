#include <avr/io.h>
#include <avr/interrupt.h>
#include "blink.h"

// Timer prescalar. Valid values are 1, 8, 64, 256, 1024
#define TIMER_PRESCALAR     256

// Compare value
#define TIMER_COMPARE       0x20

#define TICKS_SECOND        32


#if TIMER_PRESCALAR == 1
#define TIMER_PRESCALAR_VALUE   1
#elif TIMER_PRESCALAR == 8
#define TIMER_PRESCALAR_VALUE   2
#elif TIMER_PRESCALAR == 64
#define TIMER_PRESCALAR_VALUE   3
#elif TIMER_PRESCALAR == 256
#define TIMER_PRESCALAR_VALUE   4
#elif TIMER_PRESCALAR == 1024
#define TIMER_PRESCALAR_VALUE   5
#endif

#define DISPLAY_TICKS (1024 / TIMER_PRESCALAR) * (1024 / TIMER_COMPARE) / TICKS_SECOND

volatile uint8_t g_displayTick = 0;
volatile uint8_t g_displayTickCount = 0;

ISR(TIMER1_COMPA_vect) {
    if (!g_displayTickCount) {
        g_displayTick = 1;
        g_displayTickCount = DISPLAY_TICKS;
    }
    g_displayTickCount--;
}

int main()
{
    TCCR1A = 0b00000000;
    TCCR1B = 0b00001000 | TIMER_PRESCALAR_VALUE;
    TCCR1C = 0;
    OCR1A  = TIMER_COMPARE;
    TIMSK  = 0b01000000;

    DDRD = ONE << PD5;

    sei();

    for(;;) {
        while (!g_displayTick);
        blink();
        g_displayTick = 0;
    }
    return 0;
}
