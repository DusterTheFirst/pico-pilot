#pragma once

#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "pins.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    TVCCommand,
    TVCAngleRequest
} telemetry_command_type_t;

typedef struct {
    telemetry_command_type_t type;
    double *data;
} telemetry_command_t;

typedef struct {
    double tvc_x;
    double tvc_z;
    double angle;
    // uint16_t raw_temperature_voltage;
    // uint16_t raw_battery_voltage;
} cached_telemetry_data_t;

extern queue_t telemetry_queue;

void telemetry_main();

static bool telemetry_push(repeating_timer_t *rt);

static inline void telemetry_push_blocking(telemetry_command_t command) {
    queue_add_blocking(&telemetry_queue, &command);
}

static inline void telemetry_push_tvc_command(double x, double z) {
    telemetry_command_t command = {
        .type = TVCCommand,
        .data = (double[]){x, z}};

    telemetry_push_blocking(command);
}

static inline void telemetry_push_tvc_angle_request(double angle) {
    telemetry_command_t command = {
        .type = TVCAngleRequest,
        .data = (double[]){angle}};

    telemetry_push_blocking(command);
}