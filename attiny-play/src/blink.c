#include <avr/io.h>
#include "blink.h"

void blink() {
    PORTD ^= ONE << PD5;
}