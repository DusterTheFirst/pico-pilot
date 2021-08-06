#pragma once

#include <hardware/pio.h>
#include <pico/stdlib.h>

typedef struct ToneGenerator {
    uint8_t pin;
    PIO pio;
    uint state_machine;
    alarm_id_t alarm;
} tonegen_t;

#define NULL_TONEGEN ((tonegen_t){ \
    .pin = !0,                     \
})

/**
 * @brief Initialize a tonegen object to create async tones on a pin
 * 
 * @param pin The pin to generate tones on
 * @param pio The PIO block to use
 * @return tonegen_t The tonegen object to use or NULL_TONEGEN if there was no
 *                   state machine avaliable
 */
tonegen_t tonegen_init(uint8_t pin, PIO pio);
void tonegen_stop(tonegen_t *tone);
void tonegen_start(tonegen_t *tone, double frequency, uint32_t duration);
