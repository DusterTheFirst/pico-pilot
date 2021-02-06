cmake_minimum_required(VERSION 3.12)

# Include the glue for the pico SDK
include(pico_sdk_import.cmake)

# Setup the project
project(pico_pilot)

# Initialize the pico SDK
pico_sdk_init()

# Add the headers
include_directories(include)

# Add all the files to compile
file(GLOB SOURCES src/*.c) # replace .c with .cpp for a cpp project
add_executable(pico_pilot
    ${SOURCES}
)

# Setup the compile time SDK options
pico_enable_stdio_usb(pico_pilot 1)  # Enable USB Output
pico_enable_stdio_uart(pico_pilot 1) # Enable UART Output

# Build the .uf2 and other files
pico_add_extra_outputs(pico_pilot)

# Link with the pico SDK
target_link_libraries(pico_pilot
    pico_stdlib
    hardware_pwm
    pico_multicore
)
