
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

using namespace std;

class Addon {

  public:
    Addon();

    static NAN_METHOD(configure);
    static NAN_METHOD(reset);
    static NAN_METHOD(render);
    static NAN_METHOD(sleep);

  private:
};

#endif
