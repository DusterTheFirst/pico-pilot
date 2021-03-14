#include "telemetry.h"
#include "constants.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "banned.h"

// TODO: ADD DYNAMIC LOG FREQ
// The logging frequency, in hertz
const logging_frequency_t LOGGING_FREQ = LOGGING_STUPID_FAST;

queue_t telemetry_queue;

pushed_telemetry_data_t cache = {
    .tvc_x = 0.0,
    .tvc_x = 0.0,
    .angle = 0.0,
};
polled_telemetry_data_t (*telemetry_poll_callback)() = NULL;

uint32_t values_in = 0;

void telemetry_init() {
    queue_init(&telemetry_queue, sizeof(telemetry_command_t), 128);

    puts("Initial telemetry state setup.");
}

absolute_time_t ABSOLUTE_TIME_INITIALIZED_VAR(next_telemetry_push, 0);

void telemetry_main() {
    telemetry_command_t command;
    while (true) {
        gpio_xor_mask(1 << LED_PIN);

        while (queue_try_remove(&telemetry_queue, &command)) {
            switch (command.type) {
                case TVCCommand:
                    cache.tvc_x = command.tvc_command.x;
                    cache.tvc_z = command.tvc_command.z;
                    break;
                case TVCAngleRequest:
                    cache.angle = command.tvc_angle_request;
                    break;
            }
        }

        bool time_is_passed = absolute_time_diff_us(next_telemetry_push,
                                                    get_absolute_time()) <= 0;

        if (is_nil_time(next_telemetry_push) || time_is_passed) {
            telemetry_push();

            next_telemetry_push = make_timeout_time_ms(-1000000 / LOGGING_FREQ);
        }
    }
}

static bool telemetry_push() {
    polled_telemetry_data_t polled = telemetry_poll_callback();

    printf("TELEM: %f,%f,%f,%f,%f,%f,%f\n",
           (double)to_us_since_boot(get_absolute_time()) / 1000000,
           cache.tvc_x,
           cache.tvc_z,
           cache.angle,
           polled.temperature,
           polled.system_voltage,
           polled.battery_voltage);

    return true;
}
