#include "addon.h"

#include <sstream>

#define DEFAULT_TARGET_FREQ 800000
#define DEFAULT_GPIO_PIN 18
#define DEFAULT_DMA 10
#define DEFAULT_TYPE WS2811_STRIP_RGB

static ws2811_t ws2811;

NAN_METHOD(Addon::configure)
{
    Nan::HandleScope();

    // http://rgb-123.com/ws2812-color-output
    static uint8_t gammaCorrection[256] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
        2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
        6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11,
        11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18,
        19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28,
        29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40,
        40, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 53, 54,
        55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
        71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 88, 89,
        90, 91, 93, 94, 95, 96, 98, 99, 100, 102, 103, 104, 106, 107, 109, 110,
        111, 113, 114, 116, 117, 119, 120, 121, 123, 124, 126, 128, 129, 131, 132, 134,
        135, 137, 138, 140, 142, 143, 145, 146, 148, 150, 151, 153, 155, 157, 158, 160,
        162, 163, 165, 167, 169, 170, 172, 174, 176, 178, 179, 181, 183, 185, 187, 189,
        191, 193, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220,
        222, 224, 227, 229, 231, 233, 235, 237, 239, 241, 244, 246, 248, 250, 252, 255};

    if (ws2811.freq != 0)
    {
        return Nan::ThrowError("ws281x already configured.");
    }

    ws2811.freq = DEFAULT_TARGET_FREQ;
    ws2811.dmanum = DEFAULT_DMA;

    ws2811.channel[0].gpionum = DEFAULT_GPIO_PIN;
    ws2811.channel[0].count = 0;
    ws2811.channel[0].invert = 0;
    ws2811.channel[0].brightness = 255;
    ws2811.channel[0].strip_type = DEFAULT_TYPE;
    ws2811.channel[0].gamma = gammaCorrection;

    ws2811.channel[1].gpionum = 0;
    ws2811.channel[1].count = 0;
    ws2811.channel[1].invert = 0;
    ws2811.channel[1].brightness = 0;
    ws2811.channel[1].strip_type = 0;

    if (info.Length() != 1)
    {
        return Nan::ThrowError("configure requires an argument.");
    }

    v8::Local<v8::Object> options = v8::Local<v8::Object>::Cast(info[0]);

    ///////////////////////////////////////////////////////////////////////////
    // debug
    // Nan::MaybeLocal<v8::Value> debug = Nan::Get(options, Nan::New<v8::String>("debug").ToLocalChecked());

    ///////////////////////////////////////////////////////////////////////////
    // leds
    if (Nan::Has(options, Nan::New<v8::String>("leds").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_leds = Nan::Get(options, Nan::New<v8::String>("leds").ToLocalChecked());
        v8::Local<v8::Value> leds;

        if (maybe_leds.ToLocal(&leds))
            ws2811.channel[0].count = Nan::To<int>(leds).FromMaybe(ws2811.channel[0].count);
        else
            return Nan::ThrowTypeError("configure(): leds must be defined");
    }

    ///////////////////////////////////////////////////////////////////////////
    // dma
    if (Nan::Has(options, Nan::New<v8::String>("dma").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_dma = Nan::Get(options, Nan::New<v8::String>("dma").ToLocalChecked());
        v8::Local<v8::Value> dma;

        if (maybe_dma.ToLocal(&dma))
            ws2811.dmanum = Nan::To<int>(dma).FromMaybe(ws2811.dmanum);
    }

    ///////////////////////////////////////////////////////////////////////////
    // gpio
    if (Nan::Has(options, Nan::New<v8::String>("gpio").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_gpio = Nan::Get(options, Nan::New<v8::String>("gpio").ToLocalChecked());
        v8::Local<v8::Value> gpio;

        if (!maybe_gpio.IsEmpty() && maybe_gpio.ToLocal(&gpio))
        {
            ws2811.channel[0].gpionum = Nan::To<int>(gpio).FromMaybe(ws2811.channel[0].gpionum);
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // brightness
    if (Nan::Has(options, Nan::New<v8::String>("brightness").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_brightness = Nan::Get(options, Nan::New<v8::String>("brightness").ToLocalChecked());
        v8::Local<v8::Value> brightness;

        if (maybe_brightness.ToLocal(&brightness))
            ws2811.channel[0].brightness = Nan::To<int>(brightness).FromMaybe(ws2811.channel[0].brightness);
    }

    ///////////////////////////////////////////////////////////////////////////
    // stripType/strip/type
    if (true)
    {
        Nan::MaybeLocal<v8::Value> maybe_stripType;
        v8::Local<v8::Value> stripType;

        if (Nan::Has(options, Nan::New<v8::String>("strip").ToLocalChecked()).ToChecked())
            maybe_stripType = Nan::Get(options, Nan::New<v8::String>("type").ToLocalChecked());

        if (Nan::Has(options, Nan::New<v8::String>("strip").ToLocalChecked()).ToChecked())
            maybe_stripType = Nan::Get(options, Nan::New<v8::String>("strip").ToLocalChecked());

        if (Nan::Has(options, Nan::New<v8::String>("stripType").ToLocalChecked()).ToChecked())
            maybe_stripType = Nan::Get(options, Nan::New<v8::String>("stripType").ToLocalChecked());

        if (maybe_stripType.ToLocal(&stripType))
        {
            v8::String::Utf8Value value(v8::Isolate::GetCurrent(), Nan::To<v8::String>(stripType).ToLocalChecked());
            string stripTypeValue = string(*value);

            if (stripTypeValue == "rgb")
            {
                ws2811.channel[0].strip_type = WS2811_STRIP_RGB;
            }
            else if (stripTypeValue == "rbg")
            {
                ws2811.channel[0].strip_type = WS2811_STRIP_RBG;
            }
            else if (stripTypeValue == "grb")
            {
                ws2811.channel[0].strip_type = WS2811_STRIP_GRB;
            }
            else if (stripTypeValue == "gbr")
            {
                ws2811.channel[0].strip_type = WS2811_STRIP_GBR;
            }
            else if (stripTypeValue == "brg")
            {
                ws2811.channel[0].strip_type = WS2811_STRIP_BRG;
            }
            else if (stripTypeValue == "bgr")
            {
                ws2811.channel[0].strip_type = WS2811_STRIP_BGR;
            }
            else if (stripTypeValue == "grbw")
            {
                ws2811.channel[0].strip_type = SK6812_STRIP_GRBW;
            }
        }
        else
        {
            ws2811.channel[0].strip_type = WS2811_STRIP_RGB;
        }
    }
    ws2811_return_t result = ws2811_init(&ws2811);

    if (result)
    {
        std::ostringstream errortext;
        errortext << "configure(): ws2811_init() failed: " << ws2811_get_return_t_str(result);
        return Nan::ThrowError(errortext.str().c_str());
    }

    info.GetReturnValue().Set(Nan::Undefined());
};

NAN_METHOD(Addon::reset)
{
    Nan::HandleScope();

    if (ws2811.freq != 0)
    {
        memset(ws2811.channel[0].leds, 0, sizeof(uint32_t) * ws2811.channel[0].count);
        ws2811_render(&ws2811);
        ws2811_fini(&ws2811);
    }

    ws2811.freq = 0;

    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(Addon::render)
{
    Nan::HandleScope();

    if (info.Length() != 2)
    {
        return Nan::ThrowError("render() requires pixels and pixel mapping arguments.");
    }

    if (!info[0]->IsUint32Array())
        return Nan::ThrowError("render() requires pixels to be an Uint32Array.");

    if (!info[1]->IsUint32Array())
        return Nan::ThrowError("render() requires pixels mapping to be an Uint32Array.");

    v8::Local<v8::Uint32Array> array = info[0].As<v8::Uint32Array>();
    v8::Local<v8::Uint32Array> mapping = info[1].As<v8::Uint32Array>();

    // if ((uint32_t)(array->Buffer()->GetBackingStore()->ByteLength()) != (uint32_t)(4 * ws2811.channel[0].count))
    //     return Nan::ThrowError("Size of pixels does not match.");

    if ((uint32_t)(mapping->Buffer()->GetBackingStore()->ByteLength()) != (uint32_t)(4 * ws2811.channel[0].count))
        return Nan::ThrowError("Size of pixel mapping does not match.");

    uint32_t *pixels = (uint32_t *)array->Buffer()->GetBackingStore()->Data();
    uint32_t *map = (uint32_t *)mapping->Buffer()->GetBackingStore()->Data();
    uint32_t *leds = ws2811.channel[0].leds;

    for (int i = 0; i < ws2811.channel[0].count; i++)
    {
        leds[i] = pixels[map[i]];
    }

    ws2811_render(&ws2811);

    info.GetReturnValue().Set(Nan::Undefined());
};

NAN_METHOD(Addon::sleep)
{
    Nan::HandleScope();

    usleep(info[0]->ToInt32(Nan::GetCurrentContext()).ToLocalChecked()->Value() * 1000);

    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_MODULE_INIT(initAddon)
{
    Nan::SetMethod(target, "configure", Addon::configure);
    Nan::SetMethod(target, "render", Addon::render);
    Nan::SetMethod(target, "reset", Addon::reset);
    Nan::SetMethod(target, "sleep", Addon::sleep);
}

NODE_MODULE(addon, initAddon);
