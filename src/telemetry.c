#include "telemetry.h"

uint32_t bytes_in = 0;

void telemetry_main() {
    while (true) {
        gpio_put(LED_PIN, 0);
        __wfe();

        while (multicore_fifo_rvalid()) {
            gpio_put(LED_PIN, 1);

            const uint32_t fifo_in = multicore_fifo_pop_blocking();
            if (fifo_in == 0) {
                // Flush the telemetry buffer out and send it
                bytes_in = 0;
                puts("\n");
            } else {
                // Push the value out to the telemetry
                if (bytes_in > 0) {
                    puts(",");
                }

                const double *pointer = (double *)fifo_in;
                printf("%f", *pointer);
            }
        }
    }
}

void telemetry_push_blocking(const double *d) {
    multicore_fifo_push_blocking((uint32_t)d);
    __sev();
}

void telemetry_finish_blocking() {
    multicore_fifo_push_blocking(0);
    __sev();
}