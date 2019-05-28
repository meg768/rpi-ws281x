
#ifndef _addon_h
#define _addon_h

#include <nan.h>
#include <v8.h>
#include <stdexcept>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <memory.h>
#include <math.h>
#include <vector>
#include <dirent.h>
#include <getopt.h>
#include <algorithm>

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
