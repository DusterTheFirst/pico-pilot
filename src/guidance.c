#include "guidance.h"
#include "constants/adc.h"
#include "constants/pinout.h"
#include "telemetry.h"
#include "tvc.h"
#include <math.h>
#include <pico/binary_info.h>
#include <pico/double.h>
#include <stdio.h>

#include "banned.h"

tvc_servo_pair tvc;

void guidance_init(void) {
    tvc = init_tvc(PIN_PWM_SERVO_X, PIN_PWM_SERVO_Z);

    // puts("Initial guidance state setup."); TODO: add support to telem
}

void guidance_main(void) {
    // Await User Input
    const uint32_t start_command = 0xDEADBEEF;

    uint32_t shift_in = 0;
    while (shift_in != start_command) {
        int incoming = getchar();

        if (incoming != EOF) {
            shift_in = shift_in << 8 | (incoming & 0xFF);
        }

        putchar(incoming);
    }

    // Calibration
    const double calibration_moves[6][2] = {
        { -5.0, 0.0 },
        { 5.0, 0.0 },
        { 0.0, 0.0 },
        { 0.0, -5.0 },
        { 0.0, 5.0 },
        { 0.0, 0.0 }
    };

    for (uint i = 0; i < sizeof(calibration_moves) / sizeof(double[2]); i++) {
        const double *move = calibration_moves[i];

        tvc_put(&tvc, move[0], move[1]);
        sleep_ms(300);
    }

    tvc_put(&tvc, 0.0, 0.0);
    sleep_ms(1000);

    // puts("Sweeping X axis");  TODO: add support to telem

    for (double x = -5.0; x < 5.0; x += 0.5) {
        tvc_put(&tvc, x, 0.0);
        sleep_ms(40);
    }

    tvc_put(&tvc, 0.0, 0.0);
    sleep_ms(1000);

    // puts("Sweeping Z axis");  TODO: add support to telem

    for (double z = -5.0; z < 5.0; z += 0.5) {
        tvc_put(&tvc, 0.0, z);
        sleep_ms(40);
    }

    tvc_put(&tvc, 0.0, 0.0);
    sleep_ms(1000);

    double sin, cos;
    for (int i = 0; i < 100; i++)
        for (double angle = 0; angle < 2 * M_PI; angle += M_PI / 25.0) {
            sincos(angle, &sin, &cos);

            tvc_put(&tvc, sin * 5.0, cos * 5.0);
            telemetry_push_tvc_angle_request(angle);

            sleep_ms(20);
        }

    tvc_put(&tvc, 0.0, 0.0);

    // puts("Done");  TODO: add support to telem

    while (true) {
        tvc_put(&tvc, 0.0, 0.0);
        __wfi();
    }
}
