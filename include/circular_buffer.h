#pragma once

#include <stddef.h>

struct {
    void *buf;
    const size_t size;
    size_t start;
    size_t end;
} circular_buffer_t;