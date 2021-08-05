#pragma once

#include "pico/stdlib.h"

typedef struct Future {
    void (*poll)();
    bool ready;
} future_t;

typedef future_t *future_ref;

void executor_begin_polling(future_ref futures[], size_t futures_count);
