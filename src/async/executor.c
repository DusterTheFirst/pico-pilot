#include "async/executor.h"

void __attribute__((noreturn))
executor_begin_polling(future_t futures[], size_t futures_count) {
    while (true) {
        for (int i = 0; i < futures_count; i++) {
            future_t *future = &futures[i];

            if (future->ready) {
                future->poll(); // TODO: dont busy loop?
            }
        }
    }
}
