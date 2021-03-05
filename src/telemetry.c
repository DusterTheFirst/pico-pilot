#include "telemetry.h"

// The logging frequency, in hertz
const logging_frequency_t LOGGING_FREQ = LOGGING_HIGH; // TODO: ADD DYNAMIC LOG FREQ

queue_t telemetry_queue;

auto_init_mutex(cache_mutex);
cached_telemetry_data_t cache = {
    .tvc_x = 0.0,
    .tvc_x = 0.0,
    .angle = 0.0,
    // .raw_temperature_voltage = 0,
    // .raw_battery_voltage = 0.0
};

uint32_t values_in = 0;

void telemetry_main() {
    repeating_timer_t telemetry_push_timer;

    queue_init(&telemetry_queue, sizeof(telemetry_command_t), LOGGING_FREQ);

    // negative timeout means exact delay (rather than delay between callbacks)
    if (!add_repeating_timer_us(-1000000 / LOGGING_FREQ, telemetry_push, NULL, &telemetry_push_timer)) {
        printf("Failed to add telemetry push timer\n");
        return;
    }

    telemetry_command_t command;
    while (true) {
        gpio_put(LED_PIN, 0);

        while (queue_try_remove(&telemetry_queue, &command)) {
            mutex_enter_blocking(&cache_mutex);
            switch (command.type) {
                case TVCCommand:
                    cache.tvc_x = command.tvc_command.x;
                    cache.tvc_z = command.tvc_command.z;
                    break;
                case TVCAngleRequest:
                    cache.angle = command.tvc_angle_request;
                    break;
            }
            mutex_exit(&cache_mutex);
        }
    }
}

static bool telemetry_push(repeating_timer_t *rt) {
    static cached_telemetry_data_t cache_copy;

    mutex_enter_blocking(&cache_mutex);
    memcpy(&cache_copy, (const cached_telemetry_data_t *)&cache, sizeof(cached_telemetry_data_t));
    mutex_exit(&cache_mutex);

    // Shadow the global cache so that the mutex is held for as little time as possible
    // and the code does not try to access the global cache
    const cached_telemetry_data_t cache = cache_copy;

    const double conversion_factor = 3.3f / (1 << 12);
    uint16_t raw_temperature_voltage = adc_read(); // TODO: FILTER
    uint16_t raw_battery_voltage = adc_read();

    // from 4.9.4 of rp2040 datasheet
    double temperature = 27.0 - (((double)raw_temperature_voltage * conversion_factor) - 0.706) / 0.001721;

    double battery = (double)raw_battery_voltage * conversion_factor; // FIXME:

    printf("TELEM: %f,%f,%f,%f,%f\n", cache.tvc_x, cache.tvc_z, cache.angle, temperature, battery);

    return true;
}