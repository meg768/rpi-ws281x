#include "addon.h"

#define DEFAULT_TARGET_FREQ     800000
#define DEFAULT_GPIO_PIN        18
#define DEFAULT_DMA             10
#define DEFAULT_STRIP_TYPE      WS2811_STRIP_GBR

int initialized = 0;

ws2811_t ws2811;


NAN_METHOD(Addon::sleep)
{
	Nan::HandleScope();

    usleep(info[0]->Int32Value() * 1000);

    info.GetReturnValue().Set(Nan::Undefined());

}


NAN_METHOD(Addon::configure)
{
	static int initialized = 0;

	Nan::HandleScope();

	if (!initialized) {
		initialized = 1;
	}

    ws2811.freq = DEFAULT_TARGET_FREQ;
    ws2811.dmanum = DEFAULT_DMA;

    ws2811.channel[0].gpionum = DEFAULT_GPIO_PIN;
    ws2811.channel[0].count = 0;
    ws2811.channel[0].invert = 0;
    ws2811.channel[0].brightness = 255;
    ws2811.channel[0].strip_type = DEFAULT_STRIP_TYPE;

    ws2811.channel[1].gpionum = 0;
    ws2811.channel[1].count = 0;
    ws2811.channel[1].invert = 0;
    ws2811.channel[1].brightness = 255;
    ws2811.channel[1].strip_type = 0;


	if (info.Length() != 1 ) {
		return Nan::ThrowError("configure requires an argument.");
	}

	v8::Local<v8::Object> options = v8::Local<v8::Object>::Cast(info[0]);

    ///////////////////////////////////////////////////////////////////////////
    // debug
    v8::Local<v8::Value> debug = options->Get(Nan::New<v8::String>("debug").ToLocalChecked());

    ///////////////////////////////////////////////////////////////////////////
    v8::Local<v8::Value> length = options->Get(Nan::New<v8::String>("length").ToLocalChecked());

    if (!length->IsUndefined()) {
        ws2811.channel[0].count = Nan::To<int>(length).FromMaybe(ws2811.channel[0].count);
    }
    else
        return Nan::ThrowTypeError("configure(): length must be defined");


    if (ws2811_init(&ws2811))
        return Nan::ThrowError("configure(): ws2811_init() failed.");

	info.GetReturnValue().Set(Nan::Undefined());
};




NAN_METHOD(Addon::render)
{
	Nan::HandleScope();

        v8::Local<v8::Uint32Array> array = info[0].As<v8::Uint32Array>();

        void *data = array->Buffer()->GetContents().Data();
        int byteLength = array->Buffer()->GetContents().ByteLength();

        int numBytes = std::min(byteLength, 4 * ws2811.channel[0].count);

        memcpy(ws2811.channel[0].leds, data, numBytes);

        ws2811_render(&ws2811);
/*
    try {
        v8::Local<v8::Uint32Array> array = info[0].As<v8::Uint32Array>();

        void *data = array->Buffer()->GetContents().Data();
        int byteLength = array->Buffer()->GetContents().ByteLength();

        int numBytes = std::min(byteLength, 4 * ws2811.channel[0].count);

        memcpy(ws2811.channel[0].leds, data, numBytes);

        ws2811_render(&ws2811);

    }
    
    catch (exception &error) {
        string what = error.what();
        string message = string("Failed scrolling: ") + what;

		return Nan::ThrowError(message.c_str());
    }
    catch (...) {
        return Nan::ThrowError("Unhandled error");
    }
*/
	info.GetReturnValue().Set(Nan::Undefined());

};


NAN_MODULE_INIT(initAddon)
{
	Nan::SetMethod(target, "configure",  Addon::configure);
	Nan::SetMethod(target, "render",     Addon::render);
}


NODE_MODULE(addon, initAddon);