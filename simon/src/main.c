#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

uint16_t g_random = 0;

uint8_t getRandom() {
    for (uint8_t i = 0; i < 2; ++i) {
        uint8_t lsb = g_random & 1;
        g_random >>= 1;
        if (lsb || !g_random) {
            g_random ^= 0xb400;
        }
    }

    return g_random & 0xff;
}

uint8_t g_lastRandom4 = 0;

uint8_t getRandom4() {
    uint8_t value;
    do {
        value = (getRandom() & 0b00000011) + 1;
    } while (value == g_lastRandom4);
    g_lastRandom4 = value;
    return value;
}

const uint8_t LEDS[] PROGMEM = { 0, _BV(PB1) | _BV(PB4), _BV(PB3), _BV(PB1) | _BV(PB3), _BV(PB4) };

void led(uint8_t index) {
    PORTB = (PORTB & (0xff ^ (_BV(PB1) | _BV(PB3) | _BV(PB4)))) | pgm_read_byte_near(LEDS + index);
}

uint8_t g_buttonState = 0;

uint8_t getButtonsADC() {
    ADCSRA  = _BV(ADEN) | _BV(ADSC);
    while (ADCSRA & (1 << ADSC));
    const uint8_t value = ADCH;
    if (value > 224)
        return 0;
    if (value > 181)
        return 4;
    if (value > 149)
        return 3;
    if (value > 64)
        return 2;
    return 1;
}

uint8_t getButtonsPin() {
    const uint8_t button = PINB & _BV(PB2);

    if (!g_buttonState) {
        if (!button) {
            g_buttonState = 1;
            return 4;
        }
    } else if (button) {
        if (g_buttonState < 0b00010000) {
            g_buttonState = 0;
            return 1;
        }

        g_buttonState = 0;
    } else {
        if (g_buttonState & 0b00010000) {
            g_buttonState = 0b10000001;
            return 2;
        }

        if (!(g_buttonState & 0b10000000)) {
            g_buttonState += 2;
        }

        return 4;
    }

    return 0;
}

uint8_t getButtons() {
    if (ADCSRA & _BV(ADEN)) {
        return getButtonsADC();
    } else {
        return getButtonsPin();
    }
}

void setButtonsMode(uint8_t enable) {
    ADCSRA  = (enable << ADEN) | (enable << ADSC);
    DIDR0   = (enable << ADC1D);
}

volatile uint8_t g_tick = 0;

ISR(WDT_vect) {
    g_tick = 1;
}

void menu(uint8_t button);

uint8_t g_step = 0;
void(*g_stepFunc)(uint8_t) = menu;

void menu(uint8_t button) {
    getRandom();
    // led(4 - g_step / 8);
    led(button);
}

int main()
{
    DDRB    = _BV(PB0) | _BV(PB1) | _BV(PB3) | _BV(PB4);

    ADMUX   = _BV(ADLAR) | _BV(MUX0);
    ADCSRA  = 0;
    ADCSRB  = 0;

    TCCR0A  = _BV(COM0A0) | _BV(WGM01) | _BV(WGM00);
    TCCR0B  = _BV(WGM02) | _BV(CS01);
    OCR0A   = 142;

    WDTCR   = _BV(WDTIE) | _BV(WDP0);

    sei();

    while (1) {
        while (!g_tick);
        g_stepFunc(getButtons());
        if (!g_step)
            g_step = 32;
        g_step--;
        g_tick = 0;
    }
}
