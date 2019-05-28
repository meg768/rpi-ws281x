
#ifndef _addon_h
#define _addon_h

#include <nan.h>

#include <v8.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <algorithm>

#include "rpi_ws281x/ws2811.h"


class Addon {

public:
	Addon();

	static NAN_METHOD(configure);
	static NAN_METHOD(render);
	static NAN_METHOD(sleep);

private:

};

#endif
