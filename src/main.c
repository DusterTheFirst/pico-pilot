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

exp_rolling_avg_t temperature_average;
exp_rolling_avg_t system_voltage_average;
exp_rolling_avg_t battery_voltage_average;
exp_rolling_avg_t ground_offset_average;

void __attribute__((constructor)) init_averages() {
    temperature_average = exp_rolling_avg_init(.9);
    system_voltage_average = exp_rolling_avg_init(.8);
    battery_voltage_average = exp_rolling_avg_init(.8);
    ground_offset_average = exp_rolling_avg_init(.8);
}

polled_telemetry_data_t poll_voltages() {
    const double conversion_factor = 3.3 / (1 << 12);

    // ADC 1
    double filtered_ground_offset = exp_rolling_avg_push(&ground_offset_average,
                                                         adc_read());
    // FIXME: ALSO SAMPLE ADC V_REF?

    // ADC 2
    double raw_battery_voltage = ((double)adc_read() - filtered_ground_offset) *
                                 conversion_factor;
    double filtered_battery_voltage = exp_rolling_avg_push(
        &battery_voltage_average,
        raw_battery_voltage * V_BAT_CONV_FACTOR);

    // ADC 3
    double raw_system_voltage = ((double)adc_read() - filtered_ground_offset) *
                                conversion_factor;
    // from 2.1 of pico datasheet
    double filtered_system_voltage = exp_rolling_avg_push(
        &system_voltage_average,
        raw_system_voltage * 3);

    // ADC 4
    double raw_temperature_voltage = ((double)adc_read() -
                                      filtered_ground_offset) *
                                     conversion_factor;
    // from 4.9.4 of rp2040 datasheet
    double filtered_temperature = exp_rolling_avg_push(
        &temperature_average,
        27.0 - ((raw_temperature_voltage)-0.706) / 0.001721);

    return ((polled_telemetry_data_t){
        .temperature = filtered_temperature,
        .system_voltage = filtered_system_voltage,
        .battery_voltage = filtered_battery_voltage,
        .offset = filtered_ground_offset});
}

int main() {
    stdio_init_all(); // FIXME: Serial port not open on first breakpoint

    adc_init(); // FIXME: analog readings heavily dependant on VSys
                // (better calibration? use VRef?)

    adc_gpio_init(V_SYS_ADC_PIN);
    bi_decl(bi_1pin_with_name(V_SYS_ADC_PIN, "ADC, System Voltage Monitor"));
    bi_decl(bi_1pin_with_func(V_SYS_ADC_PIN, GPIO_FUNC_NULL));

    adc_gpio_init(V_BAT_ADC_PIN);
    bi_decl(bi_1pin_with_name(V_BAT_ADC_PIN, "ADC, Battery Voltage Monitor"));
    bi_decl(bi_1pin_with_func(V_BAT_ADC_PIN, GPIO_FUNC_NULL));

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
