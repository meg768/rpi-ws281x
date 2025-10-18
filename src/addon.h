
#ifndef _addon_h
#define _addon_h

#include <algorithm>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <math.h>
#include <memory.h>
#include <nan.h>
#include <signal.h>
#include <stdexcept>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
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
