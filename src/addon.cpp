#include "addon.h"

#include <sstream>
#include <string>
#include <cstdint>
#include <cstdint>
#include <algorithm>

#define DEFAULT_TARGET_FREQ 800000
#define DEFAULT_GPIO_PIN 18
#define DEFAULT_DMA 10
#define DEFAULT_TYPE WS2811_STRIP_RGB

struct config_t
{
    ws2811_t ws281x;
    int convertRGBtoRGBW;
    int initialized;
};

static config_t config;

void RGBToRGBW(uint32_t *pixels, int length)
{
    for (int i = 0; i < length; ++i)
    {
        uint32_t p = pixels[i];

        // Tolka som 0x00RRGGBB
        uint8_t r = (p >> 16) & 0xFF;
        uint8_t g = (p >> 8) & 0xFF;
        uint8_t b = p & 0xFF;

        // Beräkna vitkomponenten som minsta värdet
        uint8_t w = std::min({r, g, b});

        // Subtrahera vitt från färgerna (skydda mot underflöde)
        uint8_t r_out = (r > w) ? (r - w) : 0;
        uint8_t g_out = (g > w) ? (g - w) : 0;
        uint8_t b_out = (b > w) ? (b - w) : 0;

        // Packa som 0xWWRRGGBB
        pixels[i] = ((uint32_t)w << 24) |
                    ((uint32_t)r_out << 16) |
                    ((uint32_t)g_out << 8) |
                    ((uint32_t)b_out);
    }
}

NAN_METHOD(Addon::configure)
{
    Nan::HandleScope();

    if (config.initialized)
    {
        return Nan::ThrowError("ws281x already configured.");
    }

    memset(&config, 0, sizeof(config));

    config.ws281x.freq = DEFAULT_TARGET_FREQ;
    config.ws281x.dmanum = DEFAULT_DMA;

    config.ws281x.channel[0].gpionum = DEFAULT_GPIO_PIN;
    config.ws281x.channel[0].count = 0;
    config.ws281x.channel[0].invert = 0;
    config.ws281x.channel[0].brightness = 255;
    config.ws281x.channel[0].strip_type = DEFAULT_TYPE;
    config.ws281x.channel[0].gamma = NULL;

    config.ws281x.channel[1].gpionum = 0;
    config.ws281x.channel[1].count = 0;
    config.ws281x.channel[1].invert = 0;
    config.ws281x.channel[1].brightness = 0;
    config.ws281x.channel[1].strip_type = 0;

    if (info.Length() != 1)
    {
        return Nan::ThrowError("configure() requires an argument.");
    }

    v8::Local<v8::Object> options = v8::Local<v8::Object>::Cast(info[0]);

    ///////////////////////////////////////////////////////////////////////////
    // leds
    if (Nan::Has(options, Nan::New<v8::String>("leds").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_leds = Nan::Get(options, Nan::New<v8::String>("leds").ToLocalChecked());
        v8::Local<v8::Value> leds;

        if (maybe_leds.ToLocal(&leds))
            config.ws281x.channel[0].count = Nan::To<int>(leds).FromMaybe(config.ws281x.channel[0].count);
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
            config.ws281x.dmanum = Nan::To<int>(dma).FromMaybe(config.ws281x.dmanum);
    }

    ///////////////////////////////////////////////////////////////////////////
    // gpio
    if (Nan::Has(options, Nan::New<v8::String>("gpio").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_gpio = Nan::Get(options, Nan::New<v8::String>("gpio").ToLocalChecked());
        v8::Local<v8::Value> gpio;

        if (!maybe_gpio.IsEmpty() && maybe_gpio.ToLocal(&gpio))
        {
            config.ws281x.channel[0].gpionum = Nan::To<int>(gpio).FromMaybe(config.ws281x.channel[0].gpionum);
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // brightness
    if (Nan::Has(options, Nan::New<v8::String>("brightness").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_brightness = Nan::Get(options, Nan::New<v8::String>("brightness").ToLocalChecked());
        v8::Local<v8::Value> brightness;

        if (maybe_brightness.ToLocal(&brightness))
            config.ws281x.channel[0].brightness = Nan::To<int>(brightness).FromMaybe(config.ws281x.channel[0].brightness);
    }

    ///////////////////////////////////////////////////////////////////////////
    // rgbToRgbw
    if (Nan::Has(options, Nan::New<v8::String>("rgbToRgbw").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_rgbToRgbw = Nan::Get(options, Nan::New<v8::String>("rgbToRgbw").ToLocalChecked());
        v8::Local<v8::Value> rgbToRgbw;
        if (maybe_rgbToRgbw.ToLocal(&rgbToRgbw))
            config.convertRGBtoRGBW = Nan::To<bool>(rgbToRgbw).FromMaybe(config.convertRGBtoRGBW);
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
                config.ws281x.channel[0].strip_type = WS2811_STRIP_RGB;
            }
            else if (stripTypeValue == "grb")
            {
                config.ws281x.channel[0].strip_type = WS2811_STRIP_GRB;
            }
            else if (stripTypeValue == "gbr")
            {
                config.ws281x.channel[0].strip_type = WS2811_STRIP_GBR;
            }
            else if (stripTypeValue == "brg")
            {
                config.ws281x.channel[0].strip_type = WS2811_STRIP_BRG;
            }
            else if (stripTypeValue == "bgr")
            {
                config.ws281x.channel[0].strip_type = WS2811_STRIP_BGR;
            }

            else if (stripTypeValue == "rgbw")
            {
                config.ws281x.channel[0].strip_type = SK6812_STRIP_RGBW;
            }
            else if (stripTypeValue == "grbw")
            {
                config.ws281x.channel[0].strip_type = SK6812_STRIP_GRBW;
            }
            else if (stripTypeValue == "gbrw")
            {
                config.ws281x.channel[0].strip_type = SK6812_STRIP_GBRW;
            }
            else if (stripTypeValue == "brgw")
            {
                config.ws281x.channel[0].strip_type = SK6812_STRIP_BRGW;
            }
            else if (stripTypeValue == "bgrw")
            {
                config.ws281x.channel[0].strip_type = SK6812_STRIP_BGRW;
            }
        }
        else
        {
            config.ws281x.channel[0].strip_type = WS2811_STRIP_RGB;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // gamma
    if (Nan::Has(options, Nan::New<v8::String>("gamma").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_gamma = Nan::Get(options, Nan::New<v8::String>("gamma").ToLocalChecked());
        v8::Local<v8::Value> gamma;

        if (maybe_gamma.ToLocal(&gamma))
        {
            static uint8_t gammaCorrection[256];

            if (!node::Buffer::HasInstance(gamma))
            {
                return Nan::ThrowTypeError("configure(): gamma must be a Buffer");
            }

            if (!gamma->IsUint8Array())
            {
                return Nan::ThrowError("configure() gamma must be an Uint8Array.");
            }

            v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
            auto buffer = gamma->ToObject(context).ToLocalChecked();
            uint8_t *data = (uint8_t *)node::Buffer::Data(buffer);

            memcpy(gammaCorrection, data, sizeof(gammaCorrection));

            config.ws281x.channel[0].gamma = gammaCorrection;
        }
    }

    if (config.ws281x.channel[0].count <= 0)
    {
        return Nan::ThrowError("configure(): 'leds' must be > 0.");
    }

    ws2811_return_t result = ws2811_init(&config.ws281x);

    if (result)
    {
        std::ostringstream errortext;
        errortext << "ws2811_init() failed: " << ws2811_get_return_t_str(result);
        return Nan::ThrowError(errortext.str().c_str());
    }

    if (!config.ws281x.channel[0].leds)
    {
        return Nan::ThrowError("ws2811_init succeeded but leds buffer is null.");
    }

    config.initialized = true;

    info.GetReturnValue().Set(Nan::Undefined());
};

NAN_METHOD(Addon::reset)
{
    Nan::HandleScope();

    if (config.initialized)
    {
        if (config.ws281x.channel[0].leds && config.ws281x.channel[0].count > 0)
        {
            memset(config.ws281x.channel[0].leds, 0, sizeof(uint32_t) * config.ws281x.channel[0].count);
            ws2811_render(&config.ws281x);
        }
        ws2811_fini(&config.ws281x);
    }

    memset(&config, 0, sizeof(config));

    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(Addon::render)
{
    Nan::HandleScope();

    if (!config.initialized)
    {
        return Nan::ThrowError("ws281x not configured.");
    }
    if (info.Length() != 1)
    {
        return Nan::ThrowError("ws281x.render() requires pixels as a parameter");
    }
    if (!info[0]->IsUint32Array())
    {
        return Nan::ThrowError("ws281x.render() requires pixels to be a Uint32Array.");
    }

    // retrieve buffer from argument 1
    if (!node::Buffer::HasInstance(info[0]))
    {
        Nan::ThrowTypeError("ws281x.render() expected pixels to be a Buffer");
        return;
    }

    ws2811_channel_t channel = config.ws281x.channel[0];

    v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
    auto buffer = info[0]->ToObject(context).ToLocalChecked();
    uint32_t *data = (uint32_t *)node::Buffer::Data(buffer);

    const int numBytes = std::min(node::Buffer::Length(buffer), sizeof(ws2811_led_t) * channel.count);

    memcpy(channel.leds, data, numBytes);

    if (config.convertRGBtoRGBW)
    {
        RGBToRGBW(channel.leds, channel.count);
    }

    ws2811_render(&config.ws281x);

    info.GetReturnValue().Set(Nan::Undefined());
}

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
