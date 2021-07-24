#pragma once

#include <hardware/pio.h>
#include <pico/stdlib.h>

typedef struct Tone {
    uint8_t pin;
    PIO pio;
    int state_machine;
    alarm_id_t alarm;
} tone_t;

tone_t create_tone_generator(uint8_t pin);
void stop_tone(tone_t *tone);
void start_tone(tone_t *tone, unsigned int frequency, uint64_t duration);
