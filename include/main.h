#pragma once

#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/binary_info.h"
#include "pico/double.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "tvc.h"
#include <stdio.h>
#include <math.h>

void core1_entry();
