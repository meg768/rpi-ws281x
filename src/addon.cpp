#include "addon.h"


int initialized = 0;
ws2811_t ledstring;
ws2811_channel_t channel0data, channel1data;

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


	if (info.Length() != 1 ) {
		return Nan::ThrowError("configure requires an argument.");
	}

	v8::Local<v8::Object> options = v8::Local<v8::Object>::Cast(info[0]);

    ///////////////////////////////////////////////////////////////////////////
    // debug
    v8::Local<v8::Value> debug = options->Get(Nan::New<v8::String>("debug").ToLocalChecked());


	info.GetReturnValue().Set(Nan::Undefined());
};




NAN_METHOD(Addon::render)
{
	Nan::HandleScope();

    try {
        v8::Local<v8::Uint32Array> array = info[0].As<v8::Uint32Array>();

        void *data = array->Buffer()->GetContents().Data();
        int byteLength = array->Buffer()->GetContents().ByteLength();

        int numBytes = std::min(byteLength, 4 * ledstring.channel[0].count);

        memcpy(ledstring.channel[0].leds, data, numBytes);

        printf("A");
        ws2811_wait(&ledstring);
        printf("B");
        ws2811_render(&ledstring);
        printf("C");

    }
    
    catch (exception &error) {
        string what = error.what();
        string message = string("Failed scrolling: ") + what;

		return Nan::ThrowError(message.c_str());
    }
    catch (...) {
        return Nan::ThrowError("Unhandled error");
    }

	info.GetReturnValue().Set(Nan::Undefined());

};


NAN_MODULE_INIT(initAddon)
{
	Nan::SetMethod(target, "configure",  Addon::configure);
	Nan::SetMethod(target, "render",     Addon::render);
}


NODE_MODULE(addon, initAddon);