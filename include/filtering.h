#pragma once

#include "panic_assert.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    double value;
    bool initialized;
    double alpha;
} exp_rolling_avg_t;

static inline exp_rolling_avg_t exp_rolling_avg_init(double alpha) {
    panic_assert(alpha <= 1, "alpha value must be <1");
    panic_assert(alpha >= 0, "alpha value must be >0");

    return ((exp_rolling_avg_t){
        .value = 0,
        .initialized = false,
        .alpha = alpha});
}

static inline void exp_rolling_avg_push(exp_rolling_avg_t *self, double value) {
    // https://web.archive.org/web/20200627132729/https://www.mcgurrin.info/robots/154/
    if (self->initialized) {
        self->value = value + (self->alpha * (self->value - value));
    } else {
        self->value = value;
        self->initialized = true;
    }
}
