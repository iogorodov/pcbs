#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define TIMER_PRESCALAR     64
#define TIMER_COMPARE       125
#define TICKS_SECOND        25

#if TIMER_PRESCALAR == 1
#define TIMER_PRESCALAR_VALUE   1
#elif TIMER_PRESCALAR == 8
#define TIMER_PRESCALAR_VALUE   2
#elif TIMER_PRESCALAR == 64
#define TIMER_PRESCALAR_VALUE   3
#endif

const uint8_t DISPLAY_TICKS = (1000 / TIMER_PRESCALAR) * (1000 / TIMER_COMPARE) / TICKS_SECOND;

const uint8_t LEDS[] PROGMEM = { 0, (1 << PB1) | (1 << PB4), (1 << PB3), (1 << PB1) | (1 << PB3), (1 << PB4) };

volatile uint8_t g_displayTick = 0;
volatile uint8_t g_displayTickCount = 0;

ISR(TIM0_COMPA_vect) {
    if (!g_displayTickCount) {
        g_displayTick = 1;
        g_displayTickCount = DISPLAY_TICKS;
    }
    g_displayTickCount--;
}

void led(uint8_t index) {
    PORTB =  pgm_read_byte_near(LEDS + index);
}

uint8_t getButton() {
    return ((PINB & (1 << PB2)) == 0) ? 1 : 0;
}

uint8_t getButtons() {
    const uint8_t value = ADCH;
    if (value > 243) {
        return 0;
    } else if (value > 201) {
        return 4;
    } else if (value > 149) {
        return 3;
    } else if (value > 64) {
        return 2;
    } else {
        return 1;
    }
    // return ((PINB & (1 << PB2)) == 0) ? 1 : 0;
}

int main()
{
    DDRB = (1 << PB1) | (1 << PB3) | (1 << PB4);

    TCCR0A = 0b00000010;
    TCCR0B = 0b00000000 | TIMER_PRESCALAR_VALUE;
    OCR0A = TIMER_COMPARE - 1;
    TIMSK0 = 0b00000100;

    sei();

    ADMUX  = 0b00100001;
    ADCSRA = 0b10100100;
    ADCSRB = 0b00000011;

    while (1) {
        while (!g_displayTick);
        g_displayTick = 0;

        led(getButtons());
    }
}
