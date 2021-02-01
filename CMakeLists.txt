cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico_pilot)

pico_sdk_init()

add_executable(
    pilot
    main.c
)

pico_enable_stdio_usb(pilot 1) # Enable USB Output
pico_enable_stdio_uart(pilot 1) # Enable UART Output

pico_add_extra_outputs(pilot)

target_link_libraries(
    pilot
    pico_stdlib
    hardware_pwm
    pico_multicore
)
