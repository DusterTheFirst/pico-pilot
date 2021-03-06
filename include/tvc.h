#pragma once

#include "hardware/pwm.h"
#include "panic_assert.h"
#include "pico/stdlib.h"
#include "telemetry.h"
#include <math.h>
#include <stdio.h>

#define X_SERVO_ARM 8.6 // mm
#define Z_SERVO_ARM 8.6 // mm

#define X_MOTOR_ARM 28.1 // mm
#define Z_MOTOR_ARM 20.9 // mm

// These ratios are only usable in the approximately linear range of the
// push rod joint
// These represent the scalar to scale the inputted motor degrees to degrees to
// command the servo
#define X_ARM_RATIO X_MOTOR_ARM / X_SERVO_ARM
#define Z_ARM_RATIO Z_MOTOR_ARM / Z_SERVO_ARM

typedef struct {
    uint slice;
    uint x_channel;
    uint z_channel;
} tvc_servo_pair;

tvc_servo_pair init_tvc(uint x, uint z);
void tvc_put(tvc_servo_pair *tvc, double x, double z);
static uint16_t degrees_to_servo_command(double deg);
