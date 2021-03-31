#include "constants.h"
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
#include "telemetry.h"
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
        .offset = filtered_ground_offset});
}

int main() {
    stdio_init_all(); // FIXME: Serial port not open on first breakpoint
    // stdio_set_translate_crlf(std);
    // TODO: PICO_STDIO_ENABLE_CRLF_SUPPORT false

    adc_init(); // FIXME: analog readings heavily dependant on VSys
                // (better calibration? use VRef?)

    adc_gpio_init(V_SYS_ADC_PIN);
    bi_decl(bi_1pin_with_name(V_SYS_ADC_PIN, "ADC, System Voltage"));
    bi_decl(bi_1pin_with_func(V_SYS_ADC_PIN, GPIO_FUNC_NULL));

    adc_gpio_init(V_BAT_ADC_PIN);
    bi_decl(bi_1pin_with_name(V_BAT_ADC_PIN, "ADC, Battery Voltage"));
    bi_decl(bi_1pin_with_func(V_BAT_ADC_PIN, GPIO_FUNC_NULL));

    adc_gpio_init(GND_REF_ADC_PIN);
    bi_decl(bi_1pin_with_name(GND_REF_ADC_PIN, "ADC, Ground Reference"));
    bi_decl(bi_1pin_with_func(GND_REF_ADC_PIN, GPIO_FUNC_NULL));

    adc_set_temp_sensor_enabled(true);

    adc_select_input(1);
    adc_set_round_robin(0b11110);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));
    bi_decl(bi_1pin_with_func(LED_PIN, GPIO_FUNC_NULL));

    gpio_init(V_BUS_MONITOR_PIN);
    gpio_set_dir(V_BUS_MONITOR_PIN, GPIO_IN);
    gpio_put(V_BUS_MONITOR_PIN, 0);
    bi_decl(bi_1pin_with_name(V_BUS_MONITOR_PIN, "VBus Monitoring Pin"));
    bi_decl(bi_1pin_with_func(V_BUS_MONITOR_PIN, GPIO_FUNC_NULL));

    gpio_init(PSU_PS_PIN);
    gpio_set_dir(PSU_PS_PIN, GPIO_OUT);
    gpio_put(PSU_PS_PIN, 1); // Disable the power save mode
    bi_decl(bi_1pin_with_name(PSU_PS_PIN, "SMPS Power Save Control"));
    bi_decl(bi_1pin_with_func(PSU_PS_PIN, GPIO_FUNC_NULL));

    // Baud rate debugging
    volatile uint target_baud = TARGET_BAUD;
    volatile uint actual_baud = uart_set_baudrate(uart0, TARGET_BAUD);

    telemetry_init();
    guidance_init();

    multicore_launch_core1(guidance_main);
    telemetry_register_poll_callback(poll_voltages);

    telemetry_main();
}
