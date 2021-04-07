#pragma once

#include "pico/stdlib.h"

// from https://stackoverflow.com/a/30769646
static inline uint16_t crc16(const uint8_t *data, size_t size) {
    uint16_t crc = 0xffff;

    while (size-- > 0) {
        crc = (uint8_t)(crc >> 8) | (crc << 8);
        crc ^= *data++;
        crc ^= (uint8_t)(crc & 0xff) >> 4;
        crc ^= (crc << 8) << 4;
        crc ^= ((crc & 0xff) << 4) << 1;
    }

    return crc;
}
