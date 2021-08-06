#include "async/audio.h"
#include "async/executor.h"
#include "constants/adc.h"
#include "constants/pinout.h"
#include "constants/uart.h"
#include "filtering.h"
#include "guidance.h"
#include "pins.h"
#include "telemetry.h"
#include "tone.h"
#include "tvc.h"
#include <hardware/adc.h>
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <math.h>
#include <pico/binary_info.h>
#include <pico/double.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <stdio.h>

#include "banned.h"

// Metadata
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
bi_decl(bi_program_description(
    "Avionics system based on the Raspberry Pi Pico/RP2040 platform "));
#pragma GCC diagnostic pop

typedef struct {
    // Sampled temperature from the onboard temperature sensor
    exp_rolling_avg_t temperature;
    // System voltage
    exp_rolling_avg_t v_sys;
    // Battery voltage
    exp_rolling_avg_t v_bat;
    // ADC reading that corresponds with ground
    exp_rolling_avg_t ground_offset;
} t_sample_averages;

t_sample_averages averages;

void __attribute__((constructor)) init_averages(void) {
    averages = (t_sample_averages){
        .temperature = exp_rolling_avg_init(.9),
        .v_sys = exp_rolling_avg_init(0.85),
        .v_bat = exp_rolling_avg_init(0.85),
        .ground_offset = exp_rolling_avg_init(0.99)
    };
}

polled_telemetry_data_t poll_voltages(void) {
    const double conversion_factor = 3.3 / (1 << 12);

    // ADC 1
    double filtered_ground_offset =
        exp_rolling_avg_push(&averages.ground_offset, adc_read());

#define sample_adc_voltage() \
    (((double)adc_read() - filtered_ground_offset) * conversion_factor)

    // ADC 2
    double raw_v_bat = sample_adc_voltage();

    double filtered_v_bat =
        exp_rolling_avg_push(&averages.v_bat, raw_v_bat * V_BAT_CONV_FACTOR);

    // ADC 3
    double raw_v_sys = sample_adc_voltage();
    // from 2.1 of pico datasheet
    double filtered_v_sys =
        exp_rolling_avg_push(&averages.v_sys, raw_v_sys * 3);

    // ADC 4
    double raw_v_temp = sample_adc_voltage();
    // from 4.9.4 of rp2040 datasheet
    double filtered_temp =
        exp_rolling_avg_push(&averages.temperature,
                             27.0 - ((raw_v_temp)-0.706) / 0.001721);

    return ((polled_telemetry_data_t){
        .temperature = filtered_temp,
        .v_sys = filtered_v_sys,
        .v_bat = filtered_v_bat,
        .offset = filtered_ground_offset,
        .v_bus_present = gpio_get(PIN_V_BUS_SENSE),
    });
}

int main(void) {
    stdio_init_all(); // FIXME: Serial port not open on first breakpoint
                      // stdio_set_translate_crlf(std);

    uart_set_baudrate(uart0, TARGET_BAUD);

    adc_init(); // FIXME: analog readings heavily dependant on VSys
                // (better calibration? use VRef?)

    init_gpio_pins();
    init_adc_pins();

    future_t *audio_future = audio_system_init(PIN_PWM_BUZZER, pio0);
    audio_queue_song(AUDIO_SONG_MEGALOVANIA);

    executor_begin_polling((future_t *[1]){ audio_future }, 1);

    // telemetry_init();
    // guidance_init();

    // multicore_launch_core1(guidance_main);
    // telemetry_register_poll_callback(poll_voltages);

    // telemetry_main();
}
