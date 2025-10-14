#include "addon.h"

#include <sstream>
#include <string>
#include <cstdint>
#include <algorithm>
#include <memory>

#include <vector>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <unistd.h>

// -----------------------------------------------------------------------------
// Konfig
// -----------------------------------------------------------------------------
#define DEFAULT_TARGET_FREQ 800000
#define DEFAULT_GPIO_PIN 18
#define DEFAULT_DMA 10
#define DEFAULT_TYPE WS2811_STRIP_RGB

struct config_t
{
    ws2811_t ws281x;
    int initialized;

    // Sekventiella transitions som körs i render()
    std::vector<void (*)(uint32_t *, int)> transitions;
};

static config_t config;

// -----------------------------------------------------------------------------
// Hjälpfunktioner: WRGB pack/unpack och clamp
// -----------------------------------------------------------------------------
static inline uint8_t clamp(int v)
{
    return (v < 0) ? 0 : (v > 255 ? 255 : (uint8_t)v);
}

static inline void unpackWRGB(uint32_t p, uint8_t &w, uint8_t &r, uint8_t &g, uint8_t &b)
{
    w = (p >> 24) & 0xFF;
    r = (p >> 16) & 0xFF;
    g = (p >> 8) & 0xFF;
    b = (p) & 0xFF;
}

static inline uint32_t packWRGB(uint8_t w, uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

// -----------------------------------------------------------------------------
// Transitions (in-place på 0xWWRRGGBB)
// -----------------------------------------------------------------------------

// 1) Monochrome: gör enbart RGB gråskala, lämna W oförändrad
static void transitionMonochrome(uint32_t *px, int n)
{
    for (int i = 0; i < n; ++i)
    {
        uint8_t w, r, g, b;
        unpackWRGB(px[i], w, r, g, b);

        // Rec.709 approx: (54*r + 183*g + 19*b) / 256, +128 för rundning
        int y = (54 * r + 183 * g + 19 * b + 128) >> 8;
        uint8_t Y8 = clamp(y);

        px[i] = packWRGB(w, Y8, Y8, Y8);
    }
}

// 2) Warm white: värm tonen genom att dämpa blått och lite grönt
static void transitionWarmWhite(uint32_t *px, int n)
{
    for (int i = 0; i < n; ++i)
    {
        uint8_t w, r, g, b;
        unpackWRGB(px[i], w, r, g, b);

        int r2 = (int)(r * 1.00f + 0.0f);
        int g2 = (int)(g * 0.93f + 0.0f);
        int b2 = (int)(b * 0.75f + 0.0f);

        px[i] = packWRGB(w, clamp(r2), clamp(g2), clamp(b2));
    }
}

// 3) White shift: flytta ut "gemensam" del (min(R,G,B)) till W
//    Behåller färgton (RGB minskas lika mycket), W adderas med m
static void transitionWhiteShift(uint32_t *px, int n)
{
    for (int i = 0; i < n; ++i)
    {
        uint8_t w, r, g, b;
        unpackWRGB(px[i], w, r, g, b);

        uint8_t m = std::min({r, g, b}); // gemensam del (mängd "vitt" i RGB)

        uint8_t w2 = clamp(w + m);
        uint8_t r2 = (uint8_t)(r - m);
        uint8_t g2 = (uint8_t)(g - m);
        uint8_t b2 = (uint8_t)(b - m);

        px[i] = packWRGB(w2, r2, g2, b2);
    }
}

// -----------------------------------------------------------------------------
// Transition lookup
// -----------------------------------------------------------------------------
static void (*getTransition(const std::string &s))(uint32_t *, int)
{
    auto ieq = [](const std::string &a, const std::string &b)
    {
        if (a.size() != b.size())
            return false;
        for (size_t i = 0; i < a.size(); ++i)
            if (std::tolower((unsigned char)a[i]) != std::tolower((unsigned char)b[i]))
                return false;
        return true;
    };

    if (ieq(s, "monochrome"))
        return &transitionMonochrome;
    if (ieq(s, "warm-white"))
        return &transitionWarmWhite;
    if (ieq(s, "white-shift"))
        return &transitionWhiteShift;
    if (ieq(s, "RGBtoRGBW"))
        return &transitionWhiteShift;
    return nullptr;
}

// -----------------------------------------------------------------------------
// NAN Methods
// -----------------------------------------------------------------------------
NAN_METHOD(Addon::configure)
{
    Nan::HandleScope();

    if (config.initialized)
    {
        return Nan::ThrowError("ws281x already configured.");
    }

    // Värdeinit: nollar C-fälten och initierar std::vector korrekt
    config = config_t{};

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
        return Nan::ThrowError("ws281x.configure() - requires an argument.");
    }

    v8::Local<v8::Object> options = v8::Local<v8::Object>::Cast(info[0]);

    // leds
    if (Nan::Has(options, Nan::New<v8::String>("leds").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_leds = Nan::Get(options, Nan::New<v8::String>("leds").ToLocalChecked());
        v8::Local<v8::Value> leds;

        if (maybe_leds.ToLocal(&leds))
            config.ws281x.channel[0].count = Nan::To<int>(leds).FromMaybe(config.ws281x.channel[0].count);
        else
            return Nan::ThrowTypeError("ws281x.configure() - leds must be defined");
    }

    // dma
    if (Nan::Has(options, Nan::New<v8::String>("dma").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_dma = Nan::Get(options, Nan::New<v8::String>("dma").ToLocalChecked());
        v8::Local<v8::Value> dma;
        if (maybe_dma.ToLocal(&dma))
            config.ws281x.dmanum = Nan::To<int>(dma).FromMaybe(config.ws281x.dmanum);
    }

    // gpio
    if (Nan::Has(options, Nan::New<v8::String>("gpio").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_gpio = Nan::Get(options, Nan::New<v8::String>("gpio").ToLocalChecked());
        v8::Local<v8::Value> gpio;

        if (!maybe_gpio.IsEmpty() && maybe_gpio.ToLocal(&gpio))
            config.ws281x.channel[0].gpionum = Nan::To<int>(gpio).FromMaybe(config.ws281x.channel[0].gpionum);
    }

    // brightness
    if (Nan::Has(options, Nan::New<v8::String>("brightness").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_brightness = Nan::Get(options, Nan::New<v8::String>("brightness").ToLocalChecked());
        v8::Local<v8::Value> brightness;

        if (maybe_brightness.ToLocal(&brightness))
            config.ws281x.channel[0].brightness = Nan::To<int>(brightness).FromMaybe(config.ws281x.channel[0].brightness);
    }

    // stripType
    {
        Nan::MaybeLocal<v8::Value> maybe_stripType;
        v8::Local<v8::Value> stripType;

        if (Nan::Has(options, Nan::New<v8::String>("stripType").ToLocalChecked()).ToChecked())
            maybe_stripType = Nan::Get(options, Nan::New<v8::String>("stripType").ToLocalChecked());

        if (maybe_stripType.ToLocal(&stripType))
        {
            v8::String::Utf8Value value(v8::Isolate::GetCurrent(), Nan::To<v8::String>(stripType).ToLocalChecked());
            std::string stripTypeValue = std::string(*value);

            if (stripTypeValue == "rgb")
                config.ws281x.channel[0].strip_type = WS2811_STRIP_RGB;
            else if (stripTypeValue == "grb")
                config.ws281x.channel[0].strip_type = WS2811_STRIP_GRB;
            else if (stripTypeValue == "gbr")
                config.ws281x.channel[0].strip_type = WS2811_STRIP_GBR;
            else if (stripTypeValue == "brg")
                config.ws281x.channel[0].strip_type = WS2811_STRIP_BRG;
            else if (stripTypeValue == "bgr")
                config.ws281x.channel[0].strip_type = WS2811_STRIP_BGR;
            else if (stripTypeValue == "rgbw")
                config.ws281x.channel[0].strip_type = SK6812_STRIP_RGBW;
            else if (stripTypeValue == "grbw")
                config.ws281x.channel[0].strip_type = SK6812_STRIP_GRBW;
            else if (stripTypeValue == "gbrw")
                config.ws281x.channel[0].strip_type = SK6812_STRIP_GBRW;
            else if (stripTypeValue == "brgw")
                config.ws281x.channel[0].strip_type = SK6812_STRIP_BRGW;
            else if (stripTypeValue == "bgrw")
                config.ws281x.channel[0].strip_type = SK6812_STRIP_BGRW;
        }
        else
        {
            config.ws281x.channel[0].strip_type = WS2811_STRIP_RGB;
        }
    }

    // gamma
    if (Nan::Has(options, Nan::New<v8::String>("gamma").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_gamma = Nan::Get(options, Nan::New<v8::String>("gamma").ToLocalChecked());
        v8::Local<v8::Value> gamma;

        if (maybe_gamma.ToLocal(&gamma))
        {
            static uint8_t gammaCorrection[256];

            if (!node::Buffer::HasInstance(gamma))
                return Nan::ThrowTypeError("ws281x.configure() - gamma must be a Buffer");

            if (!gamma->IsUint8Array())
                return Nan::ThrowError("ws281x.configure() - gamma must be a Uint8Array.");

            v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
            auto buffer = gamma->ToObject(context).ToLocalChecked();
            uint8_t *data = (uint8_t *)node::Buffer::Data(buffer);

            std::memcpy(gammaCorrection, data, sizeof(gammaCorrection));
            config.ws281x.channel[0].gamma = gammaCorrection;
        }
    }

    // transitions
    {
        if (Nan::Has(options, Nan::New<v8::String>("transitions").ToLocalChecked()).ToChecked())
        {
            v8::Local<v8::Value> v = Nan::Get(options, Nan::New<v8::String>("transitions").ToLocalChecked()).ToLocalChecked();
            if (!v->IsArray())
                return Nan::ThrowTypeError("ws281x.configure() - transitions must be an Array of strings.");

            v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
            const uint32_t len = arr->Length();

            config.transitions.clear();
            config.transitions.reserve(len);

            for (uint32_t i = 0; i < len; ++i)
            {
                v8::Local<v8::Value> item = Nan::Get(arr, i).ToLocalChecked();
                if (!item->IsString())
                    return Nan::ThrowTypeError("ws281x.configure() - each transition must be a string.");

                v8::String::Utf8Value s(v8::Isolate::GetCurrent(), item);
                std::string name(*s ? *s : "");

                // debug
                fprintf(stderr, "[ws281x] transition[%u]=%s\n", i, name.c_str());

                auto fn = getTransition(name);
                if (!fn)
                {
                    std::string msg = "ws281x.configure() - unknown transition: '" + name +
                                      "'. Allowed: monochrome, warm-white, white-shift.";
                    return Nan::ThrowError(msg.c_str());
                }
                config.transitions.push_back(fn);
            }
        }
        else
        {
            config.transitions.clear();
        }
    }

    if (config.ws281x.channel[0].count <= 0)
        return Nan::ThrowError("ws281x.configure() - 'leds' must be > 0.");

    ws2811_return_t result = ws2811_init(&config.ws281x);
    if (result)
    {
        std::ostringstream errortext;
        errortext << "ws281x.configure() - ws2811_init() failed: " << ws2811_get_return_t_str(result);
        return Nan::ThrowError(errortext.str().c_str());
    }

    if (!config.ws281x.channel[0].leds)
        return Nan::ThrowError("ws281x.configure() - ws2811_init succeeded but leds buffer is null.");

    config.initialized = true;
    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(Addon::reset)
{
    Nan::HandleScope();

    if (config.initialized)
    {
        if (config.ws281x.channel[0].leds && config.ws281x.channel[0].count > 0)
        {
            std::memset(config.ws281x.channel[0].leds, 0, sizeof(uint32_t) * config.ws281x.channel[0].count);
            ws2811_render(&config.ws281x);
        }
        ws2811_fini(&config.ws281x);
    }

    // Nollställ config korrekt (initierar även vector)
    config = config_t{};

    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(Addon::render)
{
    Nan::HandleScope();

    if (!config.initialized)
        return Nan::ThrowError("ws281x not configured.");

    if (info.Length() != 1)
        return Nan::ThrowError("ws281x.render() requires pixels as a parameter");

    if (!info[0]->IsUint32Array())
        return Nan::ThrowError("ws281x.render() requires pixels to be a Uint32Array.");

    ws2811_channel_t &channel = config.ws281x.channel[0];
    if (channel.count <= 0 || channel.leds == nullptr)
        return Nan::ThrowError("ws281x.render() - LEDs buffer is not initialized.");

    v8::Local<v8::Uint32Array> arr = info[0].As<v8::Uint32Array>();
    const size_t in_len = static_cast<size_t>(arr->Length());
    const size_t led_count = static_cast<size_t>(channel.count);

    if (in_len != led_count)
        return Nan::ThrowError("ws281x.render() - pixel array length must equal configured 'leds'.");

    // Pekare till JS-backing store
    std::shared_ptr<v8::BackingStore> backing = arr->Buffer()->GetBackingStore();
    uint8_t *base = static_cast<uint8_t *>(backing->Data());
    uint32_t *data = reinterpret_cast<uint32_t *>(base + arr->ByteOffset());

    // 1) Kopiera in
    std::memcpy(channel.leds, data, led_count * sizeof(uint32_t));

    // 2) Kör filters (in-place) på 0xWWRRGGBB
    if (!config.transitions.empty())
    {
        for (auto fn : config.transitions)
        {
            fn(channel.leds, static_cast<int>(led_count));
        }
    }

    // 3) Render
    ws2811_return_t rc = ws2811_render(&config.ws281x);
    if (rc)
    {
        std::ostringstream err;
        err << "ws281x.render() - ws2811_render failed: " << ws2811_get_return_t_str(rc);
        return Nan::ThrowError(err.str().c_str());
    }

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

NODE_MODULE(addon, initAddon)
