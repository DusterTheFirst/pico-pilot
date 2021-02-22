#pragma once

#include "pico/stdlib.h"
#include <stdio.h>

typedef enum {
    FloatingPoint,
    UnsignedInteger,
    Integer
} PlotterDataType;

typedef struct {
    PlotterDataType type;
    void *value;
} PlotterData;

void plot(PlotterData all_data[], uint data_length);