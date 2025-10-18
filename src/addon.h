
#ifndef _addon_h
#define _addon_h

#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <nan.h>
#include <stdexcept>
#include <string>
#include <v8.h>
#include <vector>

extern "C" {
#include "rpi_ws281x/ws2811.h"
}

class Addon {

  public:
    Addon();

    static NAN_METHOD(configure);
    static NAN_METHOD(reset);
    static NAN_METHOD(render);
    static NAN_METHOD(sleep);

    static inline uint8_t clamp(int value);
    static inline void unpackWRGB(uint32_t p, uint8_t &w, uint8_t &r, uint8_t &g, uint8_t &b);
    static void adjustColorTemperature(uint32_t *px, int n, int kelvin);
    static void convertToRGBW(uint32_t *px, int n);

#endif
