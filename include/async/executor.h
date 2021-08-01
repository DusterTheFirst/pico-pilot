#pragma once

#include "pico/stdlib.h"

typedef struct Future {
    void (*poll)();
    bool ready;
} future_t;

void executor_begin_polling(future_t futures[], size_t futures_count);
