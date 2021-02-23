#pragma once

#include "hardware/clocks.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pins.h"
#include <stdio.h>
#include <stdlib.h>

void telemetry_main();

static inline void telemetry_push_blocking(const double *d) {
    const uint64_t * bytes = d;
    const uint32_t high_bytes = *bytes;
    const uint32_t low_bytes = bytes[4];

    multicore_fifo_push_blocking(high_bytes);
    multicore_fifo_push_blocking(low_bytes);
    __sev();
}

static inline void telemetry_finish_blocking() {
    multicore_fifo_push_blocking(0);
    __sev();
}