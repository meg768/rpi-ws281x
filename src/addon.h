
#ifndef _addon_h
#define _addon_h

/*

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
*/

#include <cstdint> // uint32_t m.m.
#include <nan.h>   // NAN_METHOD etc.
#include <v8.h>    // v8:: typer (Nan drar ofta in det ändå, men säkert)

extern "C" {
#include "rpi_ws281x/ws2811.h" // ws2811_t i klassens config
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
    // Color helpers
    static uint8_t clamp(int value);
    static uint32_t packWRGB(uint8_t white, uint8_t red, uint8_t green, uint8_t blue);
    static void unpackWRGB(uint32_t pixel, uint8_t &white, uint8_t &red, uint8_t &green, uint8_t &blue);

    // Color conversions
    static void adjustColorTemperature(uint32_t *pixels, int length);
    static void convertToRGBW(uint32_t *pixels, int length);

    static bool isRGBW();

  public:
    Addon();

    static NAN_METHOD(configure);
    static NAN_METHOD(reset);
    static NAN_METHOD(render);
    static NAN_METHOD(sleep);
};

#endif
