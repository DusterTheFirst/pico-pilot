#include "main.h"

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
    multicore_launch_core1(telemetry_main);

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

    for (int i = 0; i < 100; i++)
        for (double angle = 0; angle < 2 * M_PI; angle += M_PI / 25.0) {
            printf("T: 0,0,%f\n", angle);

            tvc_put(&tvc, sin(angle) * 5.0, cos(angle) * 5.0);
            sleep_ms(50);
        }

    tvc_put(&tvc, 0.0, 0.0);

    puts("Done");

    while (true) {
        tvc_put(&tvc, 0.0, 0.0);
        __wfi();
    }
}