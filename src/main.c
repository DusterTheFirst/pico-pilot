#include "main.h"

const uint LED_PIN = 25;

const uint SERVO_POWER_EN_PIN = 16;
const uint TVC_X_AXIS_PWM = 18;
const uint TVC_Z_AXIS_PWM = 19;

// Metadata
bi_decl(bi_program_description("Avionics system based on the Raspberry Pi Pico/RP2040 platform "));
bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));
bi_decl(bi_2pins_with_names(TVC_X_AXIS_PWM, "TVC X-Axis", TVC_Z_AXIS_PWM, "TVC Z-Axis"));
bi_decl(bi_2pins_with_func(TVC_X_AXIS_PWM, TVC_Z_AXIS_PWM, GPIO_FUNC_PWM));

tvc_servo_pair tvc;

void __attribute__((constructor)) initial_state() {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_put(LED_PIN, 0);

    tvc = init_tvc(TVC_X_AXIS_PWM, TVC_Z_AXIS_PWM);

    stdio_init_all();
    printf("\e[1;1H\e[2J");
    puts("Initial state setup.");
}

int main() {
    multicore_launch_core1(core1_entry);

    // Calibration
    tvc_put(&tvc, -5.0, 0.0);
    sleep_ms(300);
    tvc_put(&tvc, 5.0, 0.0);
    sleep_ms(300);
    tvc_put(&tvc, 0.0, 0.0);
    sleep_ms(300);
    tvc_put(&tvc, 0.0, -5.0);
    sleep_ms(300);
    tvc_put(&tvc, 0.0, 5.0);
    sleep_ms(300);
    tvc_put(&tvc, 0.0, 0.0);
    sleep_ms(300);

    puts("Sweeping X axis");

    for (double x = -5.0; x < 5.0; x += 0.1) {
        tvc_put(&tvc, x, 0.0);
        sleep_ms(50);
    }

    puts("Sweeping Z axis");

    for (double z = -5.0; z < 5.0; z += 0.1) {
        tvc_put(&tvc, 0.0, z);
        sleep_ms(50);
    }

    tvc_put(&tvc, 0.0, 0.0);

    double sin = 0.0;
    double cos = 0.0;

    for (double angle = 0; angle < 2 * M_PI; angle += M_PI / 500.0) {
        printf("T: 0,0,%f\n", angle);
        sincos(angle, &sin, &cos);

        tvc_put(&tvc, sin * 5.0, cos * 5.0);
        sleep_ms(50);
    }

    tvc_put(&tvc, 0.0, 0.0);

    puts("Done");

    while (true) {
        __wfi();
        tvc_put(&tvc, 0.0, 0.0);
    }
}

void core1_entry() {
    while (true) {
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
        gpio_put(LED_PIN, 1);

        sleep_ms(500);
    }
}