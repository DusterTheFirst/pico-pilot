cmake_minimum_required(VERSION 3.13)

# Include the glue for the pico SDK
include(deps/pico-sdk/pico_sdk_init.cmake)

# Setup the project
project(pico_pilot)

# Initialize the pico SDK
pico_sdk_init()

# Setup pinout header generation
set(PICO_REF u4)
set(NETLIST_FILE ${CMAKE_SOURCE_DIR}/deps/pico-avionics.net)
set(PINOUT_FILE ${CMAKE_SOURCE_DIR}/include/constants/pinout.h)

# https://stackoverflow.com/a/9842046
set_source_files_properties(${PINOUT_FILE} PROPERTIES GENERATED true)

# Generate the pinout header file
add_custom_command(OUTPUT ${PINOUT_FILE}
    COMMAND pinout ARGS ${NETLIST_FILE} --ref ${PICO_REF} --lang c -o ${PINOUT_FILE}
    DEPENDS ${NETLIST_FILE}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Creating ${PINOUT_FILE} with pinout")

# Add all the files to compile
file(GLOB SOURCES src/*.c src/*/*.c)
add_executable(pico_pilot
    ${SOURCES}
    ${PINOUT_FILE}
)

# Add the headers
target_include_directories(pico_pilot PUBLIC
    include
)

# Enable all warnings only for my code
target_compile_options(pico_pilot PRIVATE
    -Werror
    -Wall
    -Wextra
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wuninitialized
    -Wunused
    -Wcast-align
    -Wcast-qual
    -Wfloat-equal
    -Wmissing-format-attribute
    -Wsign-compare
    -Wstrict-prototypes
    -Wno-inline
    -Wpacked
    -Wredundant-decls
    -Wno-shadow
    -Wno-missing-field-initializers
    -Wno-missing-braces
    -Wno-multichar
    -Wundef
)

# QCBOR library
file(GLOB QCBOR_SOURCES deps/QCBOR/src/*.c)
add_library(qcbor
    ${QCBOR_SOURCES}
)
target_include_directories(qcbor SYSTEM PUBLIC
    deps/QCBOR/inc
)

# BNO055 Library
add_library(bno055
    deps/BNO055_driver/bno055.c
)
target_include_directories(bno055 SYSTEM PUBLIC
    deps/BNO055_driver
)

add_compile_definitions(
    PICO_STDIO_ENABLE_CRLF_SUPPORT=0
    _GNU_SOURCE
    QCBOR_DISABLE_FLOAT_HW_USE
    PICO_PANIC_FUNCTION=pico_panic)

# Setup the compile time SDK options
pico_enable_stdio_usb(pico_pilot 1)  # USB Output
pico_enable_stdio_uart(pico_pilot 0) # UART Output

# Build the .uf2 and other files
pico_add_extra_outputs(pico_pilot)

# Build the pioasm files
pico_generate_pio_header(pico_pilot ${CMAKE_SOURCE_DIR}/src/tone.pio)

# Link with the pico SDK
target_link_libraries(pico_pilot
    qcbor
    bno055
    pico_stdlib
    hardware_pwm
    hardware_adc
    hardware_dma
    hardware_i2c
    hardware_spi
    hardware_pio
    pico_multicore
)