#include "constants/pinout.h"
#include <hardware/adc.h>
#include <hardware/i2c.h>
#include <hardware/spi.h>
#include <pico/binary_info.h>
#include <pico/stdlib.h>

void init_gpio_pins(void) {
    // Setup onboard LED
    gpio_init(PIN_LED);
    gpio_put(PIN_LED, 0);
    gpio_set_dir(PIN_LED, GPIO_OUT);
    bi_decl(bi_1pin_with_name(PIN_LED, "On-board LED"));

    // Setup builtin V_Bus sensor
    gpio_init(PIN_V_BUS_SENSE);
    gpio_put(PIN_V_BUS_SENSE, 0);
    gpio_set_dir(PIN_V_BUS_SENSE, GPIO_IN);
    bi_decl(bi_1pin_with_name(PIN_V_BUS_SENSE, "VBus Sense Pin"));

    // Setup builtin PSU Power Select pin
    gpio_init(PIN_PSU_PS);
    gpio_put(PIN_PSU_PS, 1); // Disable the power save mode FIXME:?
    gpio_set_dir(PIN_PSU_PS, GPIO_OUT);
    bi_decl(bi_1pin_with_name(PIN_PSU_PS, "SMPS Power Save Control"));

    // Setup I2C Bus
    i2c_init(i2c0, 400000 /* Hz */); // TODO: BAUDRATE
    gpio_set_function(PIN_I2C0_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_I2C0_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_I2C0_SDA);
    gpio_pull_up(PIN_I2C0_SCL);
    bi_decl(bi_2pins_with_func(PIN_I2C0_SDA, PIN_I2C0_SCL, GPIO_FUNC_I2C));

    // Setup ARM pin
    gpio_init(PIN_ARM);
    gpio_set_dir(PIN_ARM, GPIO_IN);
    bi_decl(bi_1pin_with_name(PIN_ARM, "Avionics Arming"));

    // Setup TVC power enable
    gpio_init(PIN_SERVO_EN);
    gpio_put(PIN_SERVO_EN, 0);
    gpio_set_dir(PIN_SERVO_EN, GPIO_OUT);
    bi_decl(bi_1pin_with_name(PIN_SERVO_EN, "TVC Power Enable"));

    // Setup TVC PWM
    bi_decl(bi_2pins_with_names(PIN_PWM_SERVO_X, "TVC X-Axis",
                                PIN_PWM_SERVO_Z, "TVC Z-Axis"));
    bi_decl(bi_2pins_with_func(PIN_PWM_SERVO_X, PIN_PWM_SERVO_Z,
                               GPIO_FUNC_PWM));

    // Setup PWM Buzzer
    gpio_set_function(PIN_PWM_BUZZER, GPIO_FUNC_PWM);
    // FIXME: BUZZER INIT FN
    bi_decl(bi_1pin_with_name(PIN_PWM_BUZZER, "Buzzer"));
    bi_decl(bi_1pin_with_func(PIN_PWM_BUZZER, GPIO_FUNC_PWM));

    // Setup SPI Bus
    spi_init(spi0, 0); // TODO: BAUDRATE
    gpio_set_function(PIN_SPI0_RX, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SPI0_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SPI0_TX, GPIO_FUNC_SPI);
    bi_decl(bi_3pins_with_func(PIN_SPI0_RX, PIN_SPI0_SCK, PIN_SPI0_TX,
                               GPIO_FUNC_SPI));

    // Setup SPI FRAM Chip Select
    gpio_init(PIN_FRAM_CS);
    gpio_put(PIN_FRAM_CS, 0);
    gpio_set_dir(PIN_FRAM_CS, GPIO_OUT);
    bi_decl(bi_1pin_with_name(PIN_FRAM_CS, "SPI FRAM CS"));

    // Setup SPI BMP280 Chip Select
    gpio_init(PIN_BMP280_CS);
    gpio_put(PIN_BMP280_CS, 0);
    gpio_set_dir(PIN_BMP280_CS, GPIO_OUT);
    bi_decl(bi_1pin_with_name(PIN_BMP280_CS, "SPI BMP280 CS"));

    // Setup Parachute Deploy Pin
    gpio_init(PIN_PARACHUTE_DEPLOY);
    gpio_put(PIN_PARACHUTE_DEPLOY, 0);
    gpio_set_dir(PIN_PARACHUTE_DEPLOY, GPIO_OUT);
    bi_decl(bi_1pin_with_name(PIN_PARACHUTE_DEPLOY, "Parachute Deploy"));
}

void init_adc_pins(void) {
    adc_gpio_init(PIN_V_SYS);
    bi_decl(bi_1pin_with_name(PIN_V_SYS, "ADC, System Voltage"));
    bi_decl(bi_1pin_with_func(PIN_V_SYS, GPIO_FUNC_NULL));

    adc_gpio_init(PIN_V_BAT);
    bi_decl(bi_1pin_with_name(PIN_V_BAT, "ADC, Battery Voltage"));
    bi_decl(bi_1pin_with_func(PIN_V_BAT, GPIO_FUNC_NULL));

    adc_gpio_init(PIN_GND_REF);
    bi_decl(bi_1pin_with_name(PIN_GND_REF, "ADC, Ground Reference"));
    bi_decl(bi_1pin_with_func(PIN_GND_REF, GPIO_FUNC_NULL));

    adc_set_temp_sensor_enabled(true);

    adc_select_input(ADC_GND_REF);

    // 0b11110
    adc_set_round_robin((1 << ADC_TEMP) |
                        (1 << ADC_V_SYS) |
                        (1 << ADC_V_BAT) |
                        (1 << ADC_GND_REF));
}
