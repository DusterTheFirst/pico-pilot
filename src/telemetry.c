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
                puts("");
            } else {
                uint64_t bytes = (uint64_t)fifo_in << 32;

                const uint32_t low_bytes = multicore_fifo_pop_blocking();
                bytes &= low_bytes;

                // const double value = *(double *)(&bytes);

                // Push the value out to the telemetry
                char * prefix;
                if (values_in > 0) {
                    prefix = ",";
                } else {
                    prefix = "TELEM: ";
                }

                printf("%s%p", prefix, &bytes);
                values_in ++;
            }
        }
    }
}