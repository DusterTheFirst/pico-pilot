#include "guidance.h"

tvc_servo_pair tvc;

void guidance_init() {
    tvc = init_tvc(TVC_X_AXIS_PWM, TVC_Z_AXIS_PWM);

    puts("Initial guidance state setup.");
}

void guidance_main() {
    // Calibration
    const double calibration_moves[6][2] = {
        {-5.0, 0.0},
        {-5.0, 0.0},
        {0.0, 0.0},
        {0.0, -5.0},
        {0.0, 5.0},
        {0.0, 0.0}};

    for (int i = 0; i < sizeof(calibration_moves) / sizeof(double[2]); i++) {
        const double *move = calibration_moves[i];

        tvc_put(&tvc, move[0], move[1]);
        sleep_ms(300);
    }

    puts("Sweeping X axis");

    for (double x = -5.0; x < 5.0; x += 0.5) {
        tvc_put(&tvc, x, 0.0);
        sleep_ms(50);
    }

    puts("Sweeping Z axis");

    for (double z = -5.0; z < 5.0; z += 0.5) {
        tvc_put(&tvc, 0.0, z);
        sleep_ms(50);
    }

    tvc_put(&tvc, 0.0, 0.0);

    double sin, cos;
    for (int i = 0; i < 100; i++)
        for (double angle = 0; angle < 2 * M_PI; angle += M_PI / 25.0) {
            sincos(angle, &sin, &cos);

            tvc_put(&tvc, sin * 5.0, cos * 5.0);
            telemetry_push_tvc_angle_request(angle);

            sleep_ms(50);
        }

    tvc_put(&tvc, 0.0, 0.0);

    puts("Done");

    while (true) {
        tvc_put(&tvc, 0.0, 0.0);
        __wfi();
    }
}
