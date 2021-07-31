#include "tone.h"
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <pico/time.h>
#include <stdlib.h>

#include "tone.pio.h"

// Convert from microseconds to PIO clock cycles
static int us_to_pio_cycles(int us) {
    // Parenthesis needed to guarantee order of operations
    // to avoid 32bit overflow
    return (us * (clock_get_hz(clk_sys) / 1000000));
}

static int64_t _stop_tone_alarm(alarm_id_t id, void *user_data) {
    tone_t *tone = (tone_t *)user_data;

    tone->alarm = 0;

    stop_tone(tone);

    return 0;
}

// Safety: tone must live forever
tone_t create_tone_generator(uint8_t pin, PIO pio) {
    if (pin > 29) {
        panic("ERROR: Illegal pin in tone (%d)\n", pin);
    }

    uint offset = pio_add_program(pio, &tone_program);

    int state_machine = pio_claim_unused_sm(pio, true);

    tone_program_init(pio, state_machine, offset, pin);

    return ((tone_t){
        .pin = pin,
        .pio = pio,
        .state_machine = state_machine,
        .alarm = 0});
}

void stop_tone(tone_t *tone) {
    pio_sm_set_enabled(tone->pio, tone->state_machine, false);
    gpio_put(tone->pin, GPIO_OVERRIDE_LOW);
    gpio_set_dir(tone->pin, GPIO_OUT);
}

void start_tone(tone_t *tone, unsigned int frequency, uint64_t duration) {
    if (frequency == 0) {
        stop_tone(tone);
        return;
    }

    int us = 1000000 / frequency / 2;
    if (us < 5) {
        us = 5;
    }

    // Subtract 2 for the 2 cycles used to get the data off the FIFO
    uint32_t cycles = us_to_pio_cycles(us) - 2;

    pio_sm_put_blocking(tone->pio, tone->state_machine, cycles);
    pio_sm_set_enabled(tone->pio, tone->state_machine, true);

    if (duration != 0) {
        alarm_id_t ret = add_alarm_in_ms(duration,
                                         _stop_tone_alarm,
                                         tone,
                                         true);

        if (ret > 0) {
            tone->alarm = ret;
        } else {
            panic("ERROR: Unable to allocate timer for tone(%d, %d, %d)\n",
                  tone->pin, frequency, duration);
        }
    }
}
