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
file(GLOB SOURCES src/*.c)
file(GLOB QCBOR_SOURCES deps/QCBOR/src/*.c)
add_executable(pico_pilot
    ${SOURCES}
    ${QCBOR_SOURCES}
    deps/BNO055_driver/bno055.c
    ${PINOUT_FILE}
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