cmake_minimum_required(VERSION 3.13)

# Include the glue for the pico SDK
include(deps/pico-sdk/pico_sdk_init.cmake)

# Setup the project
project(pico_pilot)

# Initialize the pico SDK
pico_sdk_init()

# Add the header
include_directories(include)
include_directories(deps/BNO055_driver)
include_directories(deps/QCBOR/inc)

# Setup pinout header generation
set(PICO_REF u4)
set(PINOUT_FILE include/constants/pinout.h)

# https://stackoverflow.com/a/9842046
set_source_files_properties(${PINOUT_FILE} PROPERTIES GENERATED true)
add_custom_command(OUTPUT include/constants/pinout.h
    COMMAND pinout ../pico-avionics/pico-avionics.net --ref ${PICO_REF} --lang c -o ${PINOUT_FILE}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# Add all the files to compile
file(GLOB SOURCES src/*.c)
file(GLOB QCBOR_SOURCES deps/QCBOR/src/*.c)
add_executable(pico_pilot
    ${SOURCES}
    ${QCBOR_SOURCES}
    deps/BNO055_driver/bno055.c
    include/constants/pinout.h
)

add_compile_definitions(
    PICO_STDIO_ENABLE_CRLF_SUPPORT=0
    _GNU_SOURCE
    QCBOR_DISABLE_FLOAT_HW_USE)

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