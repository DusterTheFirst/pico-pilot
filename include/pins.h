#pragma once

#include "pico/binary_info.h"

const uint LED_PIN = 25;

// const uint SERVO_POWER_EN_PIN = 16; TODO: REMOVE
const uint TVC_X_AXIS_PWM = 21;
const uint TVC_Z_AXIS_PWM = 20;

// Metadata
bi_decl(bi_program_description("Avionics system based on the Raspberry Pi Pico/RP2040 platform "));
bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));
bi_decl(bi_2pins_with_names(TVC_X_AXIS_PWM, "TVC X-Axis", TVC_Z_AXIS_PWM, "TVC Z-Axis"));
bi_decl(bi_2pins_with_func(TVC_X_AXIS_PWM, TVC_Z_AXIS_PWM, GPIO_FUNC_PWM));