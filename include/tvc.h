#pragma once

#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "panic_assert.h"
#include <stdio.h>
#include <math.h>

typedef struct {
    uint slice;
    uint x_channel;
    uint z_channel;
} tvc_servo_pair;

tvc_servo_pair init_tvc(uint x, uint z);
void tvc_put(tvc_servo_pair *tvc, double x, double z);
static uint16_t degrees_to_level(double deg);