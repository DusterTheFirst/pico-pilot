// #include "constants.h"
// #include "pico/stdlib.h"
// #include <stdarg.h>
// #include <stdio.h>

// TODO: FIXME: https://github.com/raspberrypi/pico-sdk/issues/256

// void __attribute__((noreturn)) __printflike(1, 0) __wrap_panic(const char *fmt,
//                                                                ...) {
//     puts("\n*** PANIC ***\n");
//     if (fmt) {
//         va_list args;
//         va_start(args, fmt);

//         vprintf(fmt, args);
//         va_end(args);
//         puts("\n");
//     }

//     __real_panic(":D");

//     // TODO:
//     while (true) {
//         for (int i = 0; i < 3; i++) {
//             gpio_put(LED_PIN, 0);
//             sleep_ms(200);
//             gpio_put(LED_PIN, 1);
//             sleep_ms(200);
//         }
//         sleep_ms(1000);
//     }
// }
