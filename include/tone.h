#pragma once

#include <hardware/pio.h>
#include <pico/stdlib.h>

typedef struct ToneGenerator {
    uint8_t pin;
    PIO pio;
    int state_machine;
    alarm_id_t alarm;
} tonegen_t;

#define NULL_TONEGEN ((tonegen_t){ \
    .pin = !0,                     \
})

tonegen_t tonegen_init(uint8_t pin, PIO pio);
void tonegen_stop(tonegen_t *tone);
void tonegen_start(tonegen_t *tone, double frequency, uint64_t duration);
