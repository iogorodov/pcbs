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

uint8_t g_lastRandom = 0;

uint8_t getRandom4() {
    uint8_t value;
    do {
        value = (getRandom() & 0b00000011) + 1;
    } while (value == g_lastRandom);
    g_lastRandom = value;
    return value;
}

const uint8_t LEDS[] PROGMEM = { 0, _BV(PB1) | _BV(PB4), _BV(PB3), _BV(PB1) | _BV(PB3), _BV(PB4) };

void led(uint8_t index) {
    PORTB = (PORTB & (0xff ^ (_BV(PB1) | _BV(PB3) | _BV(PB4)))) | pgm_read_byte_near(LEDS + index);
}

uint8_t getButtonsADC() {
    ADCSRA  = _BV(ADEN) | _BV(ADSC);
    while (ADCSRA & (1 << ADSC));
    const uint8_t value = ADCH;
    if (value > 243)
        return 0;
    if (value > 201)
        return 4;
    if (value > 149)
        return 3;
    if (value > 64)
        return 2;
    return 1;
}

uint8_t getButtonsPin() {
    return ((PINB & (1 << PB2)) == 0) ? 1 : 0;
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

void init(uint8_t buttons);
void sleep(uint8_t buttons);
void score(uint8_t buttons);
void gameStep(uint8_t buttons);
void gameWait(uint8_t buttons);

uint8_t g_step = 0;
void(*g_stepFunc)(uint8_t) = init;

void init(uint8_t buttons) {
    if (g_step)
        return;

    led(getRandom4());

    g_step = 12;
}

void sleep(uint8_t buttons) {

}

void score(uint8_t buttons) {

}

void gameStep(uint8_t buttons) {

}

void gameWait(uint8_t buttons) {

}

volatile uint8_t g_tick = 0;

ISR(WDT_vect) {
    g_tick = 1;
}

int main()
{
    DDRB    = _BV(PB1) | _BV(PB3) | _BV(PB4);

    ADMUX   = _BV(ADLAR) | _BV(MUX0);
    ADCSRA  = 0;
    ADCSRB  = 0;

    WDTCR   = _BV(WDTIE) | _BV(WDP0);

    // setButtonsMode(1);
    sei();

    while (1) {
        while (!g_tick);
        g_stepFunc(getButtons());
        g_step--;
        g_tick = 0;
    }
}
