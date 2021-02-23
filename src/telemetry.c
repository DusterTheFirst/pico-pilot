#include "telemetry.h"

uint32_t values_in = 0;

void telemetry_main() {
    while (true) {
        gpio_put(LED_PIN, 0);
        __wfe(); // FIXME: Use interrupt handlers?

        while (multicore_fifo_rvalid()) {
            gpio_put(LED_PIN, 1);

            const uint32_t fifo_in = multicore_fifo_pop_blocking();
            if (fifo_in == 0) {
                // Flush the telemetry buffer out and send it
                values_in = 0;
                puts("\n");
            } else {
                const uint32_t high_bytes = fifo_in;
                const uint32_t low_bytes = multicore_fifo_pop_blocking();
                const uint64_t bytes = high_bytes << 32 & low_bytes;
                const double value = *(double *)(&bytes);

                // Push the value out to the telemetry
                if (values_in > 0) {
                    puts(",");
                }

                printf("%f", value);
            }
        }
    }
}