#include "tvc.h"
#include "panic_assert.h"
#include "telemetry.h"
#include <hardware/pwm.h>
#include <math.h>
#include <stdio.h>

#include "banned.h"

const uint16_t CW90 = 7500;   // 2.40ms   90* CW
const uint16_t CENTER = 4850; // 1.55ms    0*
const uint16_t CCW90 = 2200;  // 0.70ms   90* CCW

const uint TVC_PWM_MAX = 62500;
const uint TVC_CLKDIV = 40;

static uint16_t degrees_to_servo_command(double deg) {
    if (deg > 0) {
        const double steps_in_pos_90_deg = (double)(CW90 - CENTER);
        const double steps_in_pos_1_deg = steps_in_pos_90_deg / 90.0;

        const int steps_in_deg = (int)(steps_in_pos_1_deg * deg);

        // Cast happens last cause the good language of C promotes all ints to
        // the native interpretation so uint16_t + uint16_t = uint32_t cause C
        // refuses to do the smallest bit of work to downcast back to the input
        // types, so we do that here :D
        return (uint16_t)(steps_in_deg + CENTER);
    } else {
        const double steps_in_neg_90_deg = (double)(CENTER - CCW90);
        const double steps_in_neg_1_deg = steps_in_neg_90_deg / 90.0;

        const int steps_in_deg = (int)(steps_in_neg_1_deg * fabs(deg));

        // See above
        return (uint16_t)(CENTER - steps_in_deg);
    }
}

// TODO: ADD OUT OF RANGE DETECTION TO PREVENT BREAKAGE
/*
 * Initialize the given pins for servo TVC
 */
tvc_servo_pair init_tvc(uint x, uint z) {
    // Ensure the servos are separate channels but the same slice
    panic_assert(pwm_gpio_to_slice_num(x) == pwm_gpio_to_slice_num(z),
                 "TVC servo pins must be on the same pwm slice");
    panic_assert(pwm_gpio_to_channel(x) != pwm_gpio_to_channel(z),
                 "TVC servo pins must be on different pwm channels");

    tvc_servo_pair tvc = {
        .slice = pwm_gpio_to_slice_num(x),
        .x_channel = pwm_gpio_to_channel(x),
        .z_channel = pwm_gpio_to_channel(z),
    };

    gpio_set_function(x, GPIO_FUNC_PWM);
    gpio_set_function(z, GPIO_FUNC_PWM);

    uint pwm_slice = pwm_gpio_to_slice_num(x);

    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_wrap(&cfg, TVC_PWM_MAX);
    pwm_config_set_clkdiv_int(&cfg, TVC_CLKDIV); // Set 50hz
    pwm_init(pwm_slice, &cfg, true);

    // Center Servos
    pwm_set_chan_level(tvc.slice, tvc.x_channel, CENTER);
    pwm_set_chan_level(tvc.slice, tvc.z_channel, CENTER);

    return tvc;
}

/*
 * Position the TVC servos to the given positions (in degrees)
 * 
 * pos is clockwise
 * neg is counter clockwise
 */
void tvc_put(tvc_servo_pair *tvc, double x, double z) {
    panic_assert(x >= -90.0 && x <= 90.0,
                 "X AXIS ERR: "
                 "Only control signals from -90 to +90 degrees are supported");
    panic_assert(z >= -90.0 && z <= 90.0,
                 "Z AXIS ERR: "
                 "Only control signals from -90 to +90 degrees are supported");

    telemetry_push_tvc_command(x, z);

    pwm_set_chan_level(tvc->slice,
                       tvc->x_channel,
                       degrees_to_servo_command(x * X_ARM_RATIO));

    pwm_set_chan_level(tvc->slice,
                       tvc->z_channel,
                       degrees_to_servo_command(z * Z_ARM_RATIO));
}
