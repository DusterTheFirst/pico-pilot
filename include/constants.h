#pragma once

#include "pico/binary_info.h"

// V_out = (V x R2)/(R1 + R2)
// V_out((R1 + R2)/R2) = V
// (R1 + R2)/R2 = V_fac
// V_out * V_fac = V
#define voltage_divider_factor(r1, r2) ((double)(r1 + r2) / (double)r2)

static const uint LED_PIN = 25;

// const uint SERVO_POWER_EN_PIN = 16; TODO: REMOVE
static const uint TVC_X_AXIS_PWM = 21;
static const uint TVC_Z_AXIS_PWM = 20;

static const uint V_SYS_ADC_PIN = 29;
static const uint V_BAT_ADC_PIN = 28;

static const double V_BAT_CONV_FACTOR = voltage_divider_factor(220000, 100000);

static const uint TARGET_BAUD = /* 7812500 */ 115200;
