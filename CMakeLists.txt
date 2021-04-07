cmake_minimum_required(VERSION 3.12)

# Include the glue for the pico SDK
include(pico_sdk_import.cmake)

# Setup the project
project(pico_pilot)

# Initialize the pico SDK
pico_sdk_init()

# Add the headers
include_directories(include)
include_directories(deps/**)

# Add all the files to compile
file(GLOB SOURCES src/*.c) # replace .c with .cpp for a cpp project
add_executable(pico_pilot
    ${SOURCES}
    deps/BNO055_driver/bno055.c
)

add_compile_definitions(PICO_STDIO_ENABLE_CRLF_SUPPORT=false _GNU_SOURCE)

pico_wrap_function(pico_pilot panic)

# Setup the compile time SDK options
pico_enable_stdio_usb(pico_pilot 1)  # USB Output
pico_enable_stdio_uart(pico_pilot 0) # UART Output

# Build the .uf2 and other files
pico_add_extra_outputs(pico_pilot)

# Link with the pico SDK
target_link_libraries(pico_pilot
    pico_stdlib
    hardware_pwm
    hardware_adc
    hardware_dma
    pico_multicore
)
