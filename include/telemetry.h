#pragma once

#include "pico/util/queue.h"
#include <stdbool.h>

typedef enum LoggingFrequency {
    LOGGING_IDLE = 5,
    LOGGING_SLOW = 50,
    LOGGING_NORMAL = 100,
    LOGGING_FAST = 200,
    LOGGING_ULTRA_FAST = 500,
    LOGGING_STUPID_FAST = 1000,
} logging_frequency_t;

typedef enum TelemetryCommandType {
    TVCCommand,
    TVCAngleRequest
} telemetry_command_type_t;

typedef struct TelemetryCommand {
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
    double v_sys;
    double v_bat;
    double offset;
    bool v_bus_present;
} polled_telemetry_data_t;

extern queue_t telemetry_queue;
extern polled_telemetry_data_t (*telemetry_poll_callback)(void);

void telemetry_init(void);
void telemetry_main(void);

static inline void telemetry_push_blocking(telemetry_command_t command) {
    queue_add_blocking(&telemetry_queue, &command);
}

static inline void telemetry_push_tvc_command(double x, double z) {
    telemetry_command_t command = {
        .type = TVCCommand,
        .tvc_command = {
            .x = x,
            .z = z }
    };

    telemetry_push_blocking(command);
}

static inline void telemetry_push_tvc_angle_request(double angle) {
    telemetry_command_t command = {
        .type = TVCAngleRequest,
        .tvc_angle_request = angle
    };

    telemetry_push_blocking(command);
}

static inline void telemetry_register_poll_callback(
    polled_telemetry_data_t (*callback)(void)) {
    telemetry_poll_callback = callback;
}
