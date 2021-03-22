#pragma once

#include "panic_assert.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    double value;
    bool initialized;
    double alpha;
} exp_rolling_avg_t;


/// \param alpha You can choose a value for α ranging between 0 and 1.
/// At 0, the output is just the raw input: no filtering occurs. The output will
/// respond instantly both to noise and to true changes in the system. At 1, the
/// output becomes equal to the past value: it never changes, eliminating all
/// noise and all real signal changes!  Clearly one wants to select a value in
/// between these extremes.
/// \see https://web.archive.org/web/20200627132729/https://www.mcgurrin.info/robots/154/
static inline exp_rolling_avg_t exp_rolling_avg_init(double alpha) {
    panic_assert(alpha <= 1, "alpha value must be <1");
    panic_assert(alpha >= 0, "alpha value must be >0");

    return ((exp_rolling_avg_t){
        .value = 0,
        .initialized = false,
        .alpha = alpha});
}

static inline void exp_rolling_avg_push(exp_rolling_avg_t *self, double value) {
    if (self->initialized) {
        self->value = value + (self->alpha * (self->value - value));
    } else {
        self->value = value;
        self->initialized = true;
    }
}
