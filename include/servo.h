#pragma once

#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include <stdio.h>

struct Servo {
    const uint16_t POS90 = 7550;
    const uint16_t ZERO = 4850;
    const uint16_t NEG90 = 2250;
    void get();
};