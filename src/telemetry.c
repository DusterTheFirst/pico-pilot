#include "telemetry.h"

void telemetry_main() {
    while (true) {
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
        gpio_put(LED_PIN, 1);

        sleep_ms(500);
    }
}

void telemetry_push_blocking(double d) {

}