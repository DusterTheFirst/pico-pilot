#pragma once

#include <stdbool.h>
#include <stddef.h>

// #define define_filtered_value(type, name, alpha)                 \
//     static type __##name##__filtered_value;                      \
//     static bool __##name##__filtered_value__initialized = false; \
//     static double __##name##__filtered_value__alpha = alpha;

// #define update_filtered_value(name, sample)                                                                             \
//     if (__##name##__filtered_value__initialized) {                                                                      \
//         __##name##__filtered_value = sample + __##name##__filtered_value__alpha * (__##name##__filtered_value – sample) \
//     } else {                                                                                                            \
//         __##name##__filtered_value = sample                                                                             \
//     }

typedef struct {
    double value;
    bool initialized;
    double alpha;
} filtered_value_t;

static inline filtered_value_t init_filtered_value(double alpha) {
    return (filtered_value_t){
        .value = 0,
        .initialized = false,
        .alpha = alpha};
}

static inline void update_filtered_value(filtered_value_t *self, double new) {
    // https://web.archive.org/web/20200627132729/https://www.mcgurrin.info/robots/154/
    if (self->initialized) {
        self->value = new + (self->alpha * (self->value - new));
    } else {
        self->value = new;
    }
}