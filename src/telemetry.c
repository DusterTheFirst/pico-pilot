#include "telemetry.h"
#include "checksum.h"
#include "constants/pinout.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "qcbor/qcbor.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "banned.h"

// TODO: ADD DYNAMIC LOG FREQ
// The logging frequency, in hertz
const logging_frequency_t LOGGING_FREQ = LOGGING_NORMAL;
const uint64_t LOGGING_PERIOD = 1000000 / LOGGING_FREQ;

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

    // puts("Initial telemetry state setup."); TODO: add support to telem
}

absolute_time_t ABSOLUTE_TIME_INITIALIZED_VAR(next_telemetry_push, 0);

void telemetry_main() {
    telemetry_command_t command;
    while (true) {
        gpio_xor_mask(1 << PIN_LED);

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

        int64_t time_diff = absolute_time_diff_us(get_absolute_time(),
                                                  next_telemetry_push);

        if (is_nil_time(next_telemetry_push) || time_diff <= 0) {
            next_telemetry_push = make_timeout_time_us(LOGGING_PERIOD);

            telemetry_push();
        }
    }
}

UsefulBuf_MAKE_STACK_UB(cbor_buffer, 300);
// static const uint64_t MAGIC_NUMBER = 0xDEADBEEFBEEFDEAD;

static bool telemetry_push() {
    polled_telemetry_data_t polled = telemetry_poll_callback();

    QCBOREncodeContext encode_ctx;

    QCBOREncode_Init(&encode_ctx, cbor_buffer);

    QCBOREncode_OpenMap(&encode_ctx);
    QCBOREncode_AddUInt64ToMap(&encode_ctx, "running_us",
                               to_us_since_boot(get_absolute_time()));
    QCBOREncode_AddDoubleToMap(&encode_ctx, "tvc_x", cache.tvc_x);
    QCBOREncode_AddDoubleToMap(&encode_ctx, "tvc_z", cache.tvc_z);
    QCBOREncode_AddDoubleToMap(&encode_ctx, "angle", cache.angle);
    QCBOREncode_AddDoubleToMap(&encode_ctx, "temperature", polled.temperature);
    QCBOREncode_AddDoubleToMap(&encode_ctx, "v_sys", polled.v_sys);
    QCBOREncode_AddDoubleToMap(&encode_ctx, "v_bat", polled.v_bat);
    QCBOREncode_AddUInt64ToMap(&encode_ctx, "offset", polled.offset);
    QCBOREncode_AddBoolToMap(&encode_ctx, "v_bus_present",
                             polled.v_bus_present);
    QCBOREncode_CloseMap(&encode_ctx);

    UsefulBufC encoded_cbor;
    QCBORError encode_error;
    encode_error = QCBOREncode_Finish(&encode_ctx, &encoded_cbor);

    if (encode_error == QCBOR_SUCCESS) {
        fwrite(encoded_cbor.ptr, sizeof(uint8_t), encoded_cbor.len, stdout);
    } else {
        printf("oops %u\r", encode_error); // FIXME: handle better
    }

    return true;
}
