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
    LOGGING_IDLE = 5,
    LOGGING_LOW = 50,
    LOGGING_MED = 100,
    LOGGING_HIGH = 200,
} logging_frequency_t;

typedef enum {
    TVCCommand,
    TVCAngleRequest
} telemetry_command_type_t;

typedef struct {
    telemetry_command_type_t type;
    union {
        struct {
            double x;
            double z;
        } tvc_command;
        double tvc_angle_request;
    };
} telemetry_command_t;

typedef struct {
    double tvc_x;
    double tvc_z;
    double angle;
    // uint16_t raw_temperature_voltage;
    // uint16_t raw_battery_voltage;
} pushed_telemetry_data_t;

typedef struct {
    double temperature;
    double battery_voltage;
} polled_telemetry_data_t;

extern queue_t telemetry_queue;
extern polled_telemetry_data_t (*telemetry_poll_callback)();

void telemetry_main();

static bool telemetry_push(repeating_timer_t *rt);

static inline void telemetry_push_blocking(telemetry_command_t command) {
    queue_add_blocking(&telemetry_queue, &command);
}

static inline void telemetry_push_tvc_command(double x, double z) {
    telemetry_command_t command = {
        .type = TVCCommand,
        .tvc_command = {
            .x = x,
            .z = z}};

    telemetry_push_blocking(command);
}

static inline void telemetry_push_tvc_angle_request(double angle) {
    telemetry_command_t command = {
        .type = TVCAngleRequest,
        .tvc_angle_request = angle};

    telemetry_push_blocking(command);
}

static inline void telemetry_register_poll_callback(
    polled_telemetry_data_t (*callback)()) {
    telemetry_poll_callback = callback;
}
