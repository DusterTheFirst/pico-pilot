#pragma once

#include <stdarg.h>

#define panic_assert(cond, message, ...)                        \
    if (!(cond)) {                                              \
        printf("%s:%d assertion failed: ", __FILE__, __LINE__); \
        printf(message, ##__VA_ARGS__);                         \
        puts("\n\n");                                           \
        panic("ASSERTION ERROR");                               \
    }

#define panic_assert_eq(a, b)                                                 \
    if ((a) != (b)) {                                                         \
        printf("%s:%d assertion failed: %d != %d", __FILE__, __LINE__, a, b); \
        puts("\n\n");                                                         \
        panic("ASSERTION ERROR");                                             \
    }
