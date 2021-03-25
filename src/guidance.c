#define _GNU_SOURCE

#include "guidance.h"
#include "constants.h"
#include "pico/binary_info.h"
#include "pico/double.h"
#include "telemetry.h"
#include "tvc.h"
#include <math.h>
#include <stdio.h>

#include "banned.h"

tvc_servo_pair tvc;

void guidance_init() {
    tvc = init_tvc(TVC_X_AXIS_PWM, TVC_Z_AXIS_PWM);

    bi_decl(bi_2pins_with_names(TVC_X_AXIS_PWM, "TVC X-Axis",
                                TVC_Z_AXIS_PWM, "TVC Z-Axis"));
    bi_decl(bi_2pins_with_func(TVC_X_AXIS_PWM, TVC_Z_AXIS_PWM, GPIO_FUNC_PWM));

    puts("Initial guidance state setup.");
}

void guidance_main() {
    // Await User Input
    const uint32_t start_command = 0xDEADBEEF;

    uint32_t shift_in = 0;
    while (shift_in != start_command) {
        volatile int eof = EOF;
        volatile int incoming = getchar();

        if (incoming != EOF) {
            shift_in = shift_in << 8 | (incoming & 0xFF);
        }

        putchar(incoming);
    }

    // Calibration
    const double calibration_moves[6][2] = {
        {-5.0, 0.0},
        {5.0, 0.0},
        {0.0, 0.0},
        {0.0, -5.0},
        {0.0, 5.0},
        {0.0, 0.0}};

    for (int i = 0; i < sizeof(calibration_moves) / sizeof(double[2]); i++) {
        const double *move = calibration_moves[i];

        tvc_put(&tvc, move[0], move[1]);
        sleep_ms(300);
    }

    tvc_put(&tvc, 0.0, 0.0);
    sleep_ms(1000);

    puts("Sweeping X axis");

    for (double x = -5.0; x < 5.0; x += 0.5) {
        tvc_put(&tvc, x, 0.0);
        sleep_ms(40);
    }

    tvc_put(&tvc, 0.0, 0.0);
    sleep_ms(1000);

    puts("Sweeping Z axis");

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

    puts("Done");

    while (true) {
        tvc_put(&tvc, 0.0, 0.0);
        __wfi();
    }
}
