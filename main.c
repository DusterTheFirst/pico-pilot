#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/binary_info.h"
#include <stdio.h>

const uint LED_PIN = 25;
const uint TVC_X_AXIS_PWM = 18;
const uint TVC_Z_AXIS_PWM = 19;

// Metadata
bi_decl(bi_program_description("Avionics system based on the Raspberry Pi Pico/RP2040 platform "));
bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));
bi_decl(bi_2pins_with_names(TVC_X_AXIS_PWM, "TVC X-Axis", TVC_Z_AXIS_PWM, "TVC Z-Axis"));
bi_decl(bi_2pins_with_func(TVC_X_AXIS_PWM, TVC_Z_AXIS_PWM, GPIO_FUNC_PWM));

int main() {
    stdio_init_all();

    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_wrap(&cfg, 0xffff);
    // pwm_config_set_clkdiv(&cfg, 10.f);
    pwm_init(pwm_gpio_to_slice_num(TVC_X_AXIS_PWM), &cfg, true);

    gpio_set_function(TVC_X_AXIS_PWM, GPIO_FUNC_PWM);
    gpio_set_function(TVC_Z_AXIS_PWM, GPIO_FUNC_PWM);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    for (uint i = 0;; i++) {
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
        gpio_put(LED_PIN, 1);
        printf("Hello World - %i\n", i);

        // pwm_set_gpio_level(TVC_Z_AXIS_PWM, i);

        sleep_ms(500);
    }
}