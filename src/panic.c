#include "constants/pinout.h"
#include "globals.h"
#include "pico/stdlib.h"
#include <stdarg.h>
#include <stdio.h>

__attribute__((noreturn))
__printflike(1, 0) void pico_panic(const char *fmt, ...) {
    // TODO: ENTER SAFE STATE

    while (true) {
        puts("\n*** PANIC ***\n");
        if (fmt) {
            va_list args;
            va_start(args, fmt);

            vprintf(fmt, args);
            va_end(args);
            puts("\n");
        }

        bool tone_avaliable = tonegen.pin != NULL_TONEGEN.pin;

        for (int x = 0; x < 10; x++) {
            for (int i = 4000; i > 3000; i -= 100) {
                if (tone_avaliable) {
                    tonegen_start(&tonegen, (double)i, 0);
                }
                gpio_put(PIN_LED, 0);
                sleep_ms(50);
                gpio_put(PIN_LED, 1);
                sleep_ms(50);
            }
        }
    }
}
