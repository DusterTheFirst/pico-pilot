#include "main.h"
#include "banned.h"

// Metadata
bi_decl(bi_program_description(
    "Avionics system based on the Raspberry Pi Pico/RP2040 platform "));
bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));
bi_decl(bi_2pins_with_names(TVC_X_AXIS_PWM, "TVC X-Axis",
                            TVC_Z_AXIS_PWM, "TVC Z-Axis"));
bi_decl(bi_2pins_with_func(TVC_X_AXIS_PWM, TVC_Z_AXIS_PWM, GPIO_FUNC_PWM));
bi_decl(bi_2pins_with_func(V_SYS_ADC_PIN, V_BAT_ADC_PIN, GPIO_FUNC_NULL));

tvc_servo_pair tvc;

void __attribute__((constructor)) initial_state() {
    adc_init();

    adc_gpio_init(V_SYS_ADC_PIN);
    adc_gpio_init(V_BAT_ADC_PIN);
    adc_set_temp_sensor_enabled(true);

    adc_select_input(2);
    adc_set_round_robin(0b11100);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_put(LED_PIN, 0);

    tvc = init_tvc(TVC_X_AXIS_PWM, TVC_Z_AXIS_PWM);

    stdio_init_all();

    for (int i = 0; i < 10; i++)
        printf("\n");
    printf("\e[1;1H\e[2J"); // Clear the screen

    puts("Initial state setup.");
}

exp_rolling_avg_t poll_temperature;
exp_rolling_avg_t poll_system_voltage;
exp_rolling_avg_t poll_battery_voltage;

void __attribute__((constructor)) init_averages() {
    poll_temperature = exp_rolling_avg_init(.9);
    poll_system_voltage = exp_rolling_avg_init(.6);
    poll_battery_voltage = exp_rolling_avg_init(.6);
}

polled_telemetry_data_t poll_voltages() {
    const double conversion_factor = 3.3 / (1 << 12);

    // ADC 2
    double raw_battery_voltage = (double)adc_read() * conversion_factor;
    // ADC 3
    double raw_system_voltage = (double)adc_read() * conversion_factor;
    // ADC 4
    double raw_temperature_voltage = (double)adc_read() * conversion_factor;

    // from 4.9.4 of rp2040 datasheet
    exp_rolling_avg_push(&poll_temperature,
                         27.0 - ((raw_temperature_voltage)-0.706) / 0.001721);

    // from 2.1 of pico datasheet
    exp_rolling_avg_push(&poll_system_voltage, raw_system_voltage * 3);

    exp_rolling_avg_push(&poll_battery_voltage,
                         raw_battery_voltage * V_BAT_CONV_FACTOR);

    return ((polled_telemetry_data_t){
        .temperature = poll_temperature.value,
        .system_voltage = poll_system_voltage.value,
        .battery_voltage = poll_battery_voltage.value});
}

int main() {
    multicore_launch_core1(telemetry_main);
    telemetry_register_poll_callback(poll_voltages);

    // Calibration
    const double calibration_moves[6][2] = {
        {-5.0, 0.0},
        {-5.0, 0.0},
        {0.0, 0.0},
        {0.0, -5.0},
        {0.0, 5.0},
        {0.0, 0.0}};

    for (int i = 0; i < sizeof(calibration_moves) / sizeof(double[2]); i++) {
        const double *move = calibration_moves[i];

        tvc_put(&tvc, move[0], move[1]);
        sleep_ms(300);
    }

    puts("Sweeping X axis");

    for (double x = -5.0; x < 5.0; x += 0.5) {
        tvc_put(&tvc, x, 0.0);
        sleep_ms(50);
    }

    puts("Sweeping Z axis");

    for (double z = -5.0; z < 5.0; z += 0.5) {
        tvc_put(&tvc, 0.0, z);
        sleep_ms(50);
    }

    tvc_put(&tvc, 0.0, 0.0);

    double sin, cos;
    for (int i = 0; i < 100; i++)
        for (double angle = 0; angle < 2 * M_PI; angle += M_PI / 25.0) {
            sincos(angle, &sin, &cos);

            tvc_put(&tvc, sin * 5.0, cos * 5.0);
            telemetry_push_tvc_angle_request(angle);

            sleep_ms(50);
        }

    tvc_put(&tvc, 0.0, 0.0);

    puts("Done");

    while (true) {
        tvc_put(&tvc, 0.0, 0.0);
        __wfi();
    }
}
