
#ifndef _addon_h
#define _addon_h

// C++ standard
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// NAN/V8 och ditt eget
#include "addon.h"
#include <nan.h>

// C/POSIX – sist
#include <cctype>
#include <cstdio>
#include <unistd.h>

extern "C" {
#include "rpi_ws281x/ws2811.h"
}

#define DEFAULT_TARGET_FREQ 800000
#define DEFAULT_GPIO_PIN 18
#define DEFAULT_DMA 10
#define DEFAULT_TYPE WS2811_STRIP_RGB

class Addon {
    struct config_t {
        // Initialization flag
        int initialized;

        // Color temperature in Kelvin (0 == disabled)
        int colorTemperature;

        // Bypass all processing when using an RGBW strip (raw 0xWWRRGGBB)
        int rawRGBW;

        // Underlying ws281x struct
        ws2811_t ws281x;
    };

    static config_t config;

  private:
    static inline uint8_t clamp(int value);
    static inline void unpackWRGB(uint32_t p, uint8_t &w, uint8_t &r, uint8_t &g, uint8_t &b);
    static void adjustColorTemperature(uint32_t *px, int n);
    static void convertToRGBW(uint32_t *px, int n);
    static uint32_t packWRGB(uint8_t w, uint8_t r, uint8_t g, uint8_t b);
    static bool isRGBW();

  public:
    Addon();

    static NAN_METHOD(configure);
    static NAN_METHOD(reset);
    static NAN_METHOD(render);
    static NAN_METHOD(sleep);
};

#endif
