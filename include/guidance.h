#pragma once

#define _GNU_SOURCE

#include "pico/double.h"
#include "pins.h"
#include "tvc.h"
#include <math.h>
#include <stdio.h>

void guidance_init();
void guidance_main();
