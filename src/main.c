#include "constants/adc.h"
#include "constants/pinout.h"
#include "constants/uart.h"
#include "filtering.h"
#include "guidance.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/binary_info.h"
#include "pico/double.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pins.h"
#include "telemetry.h"
#include "tone.h"
#include "tvc.h"
#include <math.h>
#include <stdio.h>

#include "banned.h"

// Metadata
bi_decl(bi_program_description(
    "Avionics system based on the Raspberry Pi Pico/RP2040 platform "));

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

void __attribute__((constructor)) init_averages() {
    averages = (t_sample_averages){
        .temperature = exp_rolling_avg_init(.9),
        .v_sys = exp_rolling_avg_init(0.85),
        .v_bat = exp_rolling_avg_init(0.85),
        .ground_offset = exp_rolling_avg_init(0.99)};
}

polled_telemetry_data_t poll_voltages() {
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
        .v_bus_present = gpio_get(PIN_V_BUS_SENSE)});
}

tonegen_t tonegen = NULL_TONEGEN;

int main() {
    stdio_init_all(); // FIXME: Serial port not open on first breakpoint
                      // stdio_set_translate_crlf(std);

    uart_set_baudrate(uart0, TARGET_BAUD);

    adc_init(); // FIXME: analog readings heavily dependant on VSys
    // (better calibration? use VRef?)

    init_gpio_pins();
    init_adc_pins();

    tonegen = tonegen_init(PIN_PWM_BUZZER, pio0);

    // panic("uh oh!");

    // tonegen_start(&tonegen, 493.8833, 3000);
    // sleep_ms(3000);
    // tonegen_start(&tonegen, 493.8833, 500);
    // sleep_ms(500);
    // tonegen_start(&tonegen, 440.0000, 3000);
    // sleep_ms(3000);
    // tonegen_start(&tonegen, 493.8833, 500);
    // sleep_ms(500);
    // tonegen_start(&tonegen, 415.3047, 3000);
    // sleep_ms(3000);
    // tonegen_start(&tonegen, 369.9944, 500);
    // sleep_ms(500);
    // tonegen_start(&tonegen, 329.6276, 500);
    // sleep_ms(500);
    // tonegen_start(&tonegen, 369.9944, 1000);
    // sleep_ms(1000);

    volatile int test = clock_get_hz(clk_sys);

    tonegen_start(&tonegen, 440, 0);
    while (true) {
    }

    // int first[] = {293, 262, 247, 233};

    // while (true) {
    //     for (int i = 0; i < sizeof(first) / sizeof(first[0]); i++) {
    //         start_tone(&tonegen, first[i], 150);
    //         sleep_ms(200);
    //         start_tone(&tonegen, first[i], 0);
    //         sleep_ms(200);
    //         start_tone(&tonegen, 587, 200);
    //         sleep_ms(400);
    //         start_tone(&tonegen, 440, 400);
    //         sleep_ms(600);
    //         start_tone(&tonegen, 415, 200);
    //         sleep_ms(400);
    //         start_tone(&tonegen, 392, 200);
    //         sleep_ms(400);
    //         start_tone(&tonegen, 349, 0);
    //         sleep_ms(400);
    //         start_tone(&tonegen, 293, 0);
    //         sleep_ms(200);
    //         start_tone(&tonegen, 349, 0);
    //         sleep_ms(200);
    //         start_tone(&tonegen, 392, 0);
    //         sleep_ms(200);
    //         stop_tone(&tonegen);
    //     }
    // }

    // telemetry_init();
    // guidance_init();

    // multicore_launch_core1(guidance_main);
    // telemetry_register_poll_callback(poll_voltages);

    // telemetry_main();
}
