#include "tone.h"
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <math.h>
#include <pico/time.h>
#include <stdlib.h>

#include "tone.pio.h"

static int64_t _stop_tone_alarm(alarm_id_t id, void *user_data) {
    tonegen_t *tone = (tonegen_t *)user_data;

    tone->alarm = 0;

    tonegen_stop(tone);

    return 0;
}

// Safety: tone must live 'static
tonegen_t tonegen_init(uint8_t pin, PIO pio) {
    if (pin > 29) {
        panic("ERROR: Illegal pin in tone (%d)\n", pin);
    }

    uint offset = pio_add_program(pio, &tone_program);

    int state_machine = pio_claim_unused_sm(pio, true);

    tone_program_init(pio, state_machine, offset, pin);

    return ((tonegen_t){
        .pin = pin,
        .pio = pio,
        .state_machine = state_machine,
        .alarm = 0,
    });
}

void tonegen_stop(tonegen_t *tone) {
    pio_sm_set_enabled(tone->pio, tone->state_machine, false);
    gpio_put(tone->pin, GPIO_OVERRIDE_LOW);
    gpio_set_dir(tone->pin, GPIO_OUT);
}

void tonegen_start(tonegen_t *tone, double frequency, uint32_t duration) {
    if (frequency == 0) {
        tonegen_stop(tone);
        return;
    }

    // Get the frequency of half of the pulse, which happens twice per period
    double double_freq = frequency * 2;

    double cycles_d = (double)clock_get_hz(clk_sys) / double_freq;

    // Subtract 2 for the 2 cycles used to get the data off the FIFO
    uint32_t cycles = (uint32_t)round(cycles_d) - 2;

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
