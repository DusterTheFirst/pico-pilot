#include "main.h"

const uint LED_PIN = 25;

const uint SERVO_POWER_EN_PIN = 16;
const uint TVC_X_AXIS_PWM = 18;
const uint TVC_Z_AXIS_PWM = 19;

// Metadata
bi_decl(bi_program_description("Avionics system based on the Raspberry Pi Pico/RP2040 platform "));
bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));
bi_decl(bi_2pins_with_names(TVC_X_AXIS_PWM, "TVC X-Axis", TVC_Z_AXIS_PWM, "TVC Z-Axis"));
bi_decl(bi_2pins_with_func(TVC_X_AXIS_PWM, TVC_Z_AXIS_PWM, GPIO_FUNC_PWM));

int main() {
    stdio_init_all();
    printf("\e[1;1H\e[2J");

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);

    gpio_init(SERVO_POWER_EN_PIN);
    gpio_set_dir(SERVO_POWER_EN_PIN, GPIO_OUT);
    gpio_put(SERVO_POWER_EN_PIN, 0);

    multicore_launch_core1(core1_entry);

    tvc_servo_pair tvc = init_tvc(TVC_X_AXIS_PWM, TVC_Z_AXIS_PWM);

    gpio_put(SERVO_POWER_EN_PIN, 1);

    // uint32_t pwm_clock_freq = clock_get_hz(clk_sys) / PWM_CLOCK_DIV;
    // double clock_period_ms = 1000.0 / (double)pwm_clock_freq;
    // double cycle_period_ms = clock_period_ms * (double)PWM_WRAP;

    // printf("PWM clock running at %i MHz (%fms per cycle) (%fus per clock)\n", pwm_clock_freq / 1000000, cycle_period_ms, clock_period_ms * 1000);

    // uint16_t approx_center = (uint16_t)((1.5 / cycle_period_ms) * (double)PWM_WRAP);
    // uint16_t approx_low = (uint16_t)((1.0 / cycle_period_ms) * (double)PWM_WRAP);
    // uint16_t approx_high = (uint16_t)((2.0 / cycle_period_ms) * (double)PWM_WRAP);

    // printf("Approx servo extents: (%u <- %u -> %u)\n", approx_low, approx_center, approx_high);

    // volatile int min_offset = 0;
    // volatile int center_offset = 0;
    // volatile int max_offset = 0;

    // while (true) {
    // printf("Servo offsets (%i <- %i -> %i)\n", min_offset, center_offset, max_offset);

    // uint16_t center = approx_center + center_offset;
    // uint16_t min = center + min_offset;
    // uint16_t max = center + max_offset;

    // printf("Servo absolute extents (%u <- %u -> %u)\n", min, center, max);

    const uint16_t RIGHT = 7500;  // 2.40ms    90* CW
    const uint16_t CENTER = 4850; // 1.55ms     0*
    const uint16_t LEFT = 2200;   // 0.70ms    90* CCW

    const uint16_t RIGHT10 = (uint16_t)(((double)(RIGHT - CENTER) / 90.0) * 10.0) + CENTER;
    const uint16_t LEFT10 = CENTER - (uint16_t)(((double)(CENTER - LEFT) / 90.0) * 10.0);

    uint16_t pos = CENTER;
    int16_t direction = 1;

    while (true) {
        tvc_put(&tvc, 0.0, 0.0);
        // pwm_set_gpio_level(TVC_X_AXIS_PWM, pos);
        // pwm_set_gpio_level(TVC_Z_AXIS_PWM, pos);

        // pos += direction;

        // if (pos <= LEFT10) {
        //     direction = 1;
        //     puts("Direction +");
        // } else if (pos >= RIGHT10) {
        //     direction = -1;
        //     puts("Direction -");
        // }

        // sleep_ms(1);
    }

    //     // Calibration
    //     puts("Sweeping X axis");

    //     for (int16_t x = min; x < max; x++) {
    //         pwm_set_gpio_level(TVC_X_AXIS_PWM, x);
    //         sleep_us(500);
    //     }

    //     puts("Sweeping Z axis");

    //     for (int16_t z = min; z < max; z++) {
    //         pwm_set_gpio_level(TVC_Z_AXIS_PWM, z);
    //         sleep_us(500);
    //     }

    //     // for (uint i = 0;; i++) {
    //     //     gpio_put(LED_PIN, 0);
    //     //     sleep_ms(250);
    //     //     gpio_put(LED_PIN, 1);
    //     //     printf("Hello World - %i\n", i);

    //     //     // pwm_set_gpio_level(TVC_Z_AXIS_PWM, i);

    //     //     sleep_ms(500);
    //     // }

    puts("Done");
    // }

    while (true) {
        __wfi();
    }
}

void core1_entry() {
    while (true) {
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
        gpio_put(LED_PIN, 1);

        sleep_ms(500);
    }
}