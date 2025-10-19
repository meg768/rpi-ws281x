#include "addon.h"

uint8_t Addon::clamp(int v) {
    return (v < 0) ? 0 : (v > 255 ? 255 : (uint8_t)v);
}

void Addon::unpackWRGB(uint32_t p, uint8_t &w, uint8_t &r, uint8_t &g, uint8_t &b) {
    w = (p >> 24) & 0xFF;
    r = (p >> 16) & 0xFF;
    g = (p >> 8) & 0xFF;
    b = (p) & 0xFF;
}

uint32_t Addon::packWRGB(uint8_t w, uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)w << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

bool Addon::isRGBW() {
    switch (config.ws281x.channel[0].strip_type) {
        case SK6812_STRIP_RGBW:
        case SK6812_STRIP_GRBW:
        case SK6812_STRIP_GBRW:
        case SK6812_STRIP_BRGW:
        case SK6812_STRIP_BGRW:
            return true;
        default:
            return false;
    }
}

// -----------------------------------------------------------------------------
// Color temperature adjustment (RGB only; W is left untouched)
// -----------------------------------------------------------------------------
void Addon::adjustColorTemperature(uint32_t *px, int n) {

    int kelvin = config.colorTemperature;

    // Defensive no-op
    if (kelvin <= 0)
        return;

    // Constrain to reasonable range
    if (kelvin < 1000)
        kelvin = 1000;
    if (kelvin > 40000)
        kelvin = 40000;

    // Kelvin -> RGB factors
    float t = kelvin / 100.0f;
    float rF, gF, bF;

    if (t <= 66.0f) {
        rF = 1.0f;
        gF = 0.3900815788f * logf(t) - 0.6318414438f;
        bF = (t <= 19.0f) ? 0.0f : (0.5432067891f * logf(t - 10.0f) - 1.1962540891f);
    } else {
        rF = 1.2929361861f * powf(t - 60.0f, -0.1332047592f);
        gF = 1.1298908609f * powf(t - 60.0f, -0.0755148492f);
        bF = 1.0f;
    }

    // Clamp factors to [0..1]
    rF = (rF < 0.0f) ? 0.0f : (rF > 1.0f ? 1.0f : rF);
    gF = (gF < 0.0f) ? 0.0f : (gF > 1.0f ? 1.0f : gF);
    bF = (bF < 0.0f) ? 0.0f : (bF > 1.0f ? 1.0f : bF);

    // Apply per pixel (RGB only)
    for (int i = 0; i < n; ++i) {
        uint8_t w, r, g, b;
        unpackWRGB(px[i], w, r, g, b);

        int r2 = (int)std::lround(r * rF);
        int g2 = (int)std::lround(g * gF);
        int b2 = (int)std::lround(b * bF);

        px[i] = packWRGB(w, Addon::clamp(r2), Addon::clamp(g2), Addon::clamp(b2));
    }
}

// -----------------------------------------------------------------------------
// RGB -> RGBW conversion (extract common white component)
// -----------------------------------------------------------------------------
void Addon::convertToRGBW(uint32_t *px, int n) {
    if (!isRGBW())
        return;

    for (int i = 0; i < n; ++i) {
        uint8_t w, r, g, b;
        unpackWRGB(px[i], w, r, g, b);

        // Common white component (portable min-min, no initializer_list)
        uint8_t m = std::min(std::min(r, g), b);

        // Move common component to W and subtract from RGB
        uint8_t w2 = Addon::clamp((int)w + (int)m);
        px[i] = packWRGB(w2, (uint8_t)(r - m), (uint8_t)(g - m), (uint8_t)(b - m));
    }
}

// -----------------------------------------------------------------------------
// NAN Methods
// -----------------------------------------------------------------------------
NAN_METHOD(Addon::configure) {
    Nan::HandleScope();

    if (config.initialized) {
        return Nan::ThrowError("ws281x already configured.");
    }

    // Value-init config (zero POD, clear pointers)
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

    if (info.Length() != 1) {
        return Nan::ThrowError("ws281x.configure() requires an options object.");
    }

    v8::Local<v8::Object> options = v8::Local<v8::Object>::Cast(info[0]);

    // leds
    if (Nan::Has(options, Nan::New("leds").ToLocalChecked()).ToChecked()) {
        Nan::MaybeLocal<v8::Value> maybe_leds = Nan::Get(options, Nan::New("leds").ToLocalChecked());
        v8::Local<v8::Value> leds;
        if (maybe_leds.ToLocal(&leds))
            config.ws281x.channel[0].count = Nan::To<int>(leds).FromMaybe(config.ws281x.channel[0].count);
        else
            return Nan::ThrowTypeError("ws281x.configure() - 'leds' must be defined");
    }

    // dma
    if (Nan::Has(options, Nan::New("dma").ToLocalChecked()).ToChecked()) {
        Nan::MaybeLocal<v8::Value> maybe_dma = Nan::Get(options, Nan::New("dma").ToLocalChecked());
        v8::Local<v8::Value> dma;
        if (maybe_dma.ToLocal(&dma))
            config.ws281x.dmanum = Nan::To<int>(dma).FromMaybe(config.ws281x.dmanum);
    }

    // gpio
    if (Nan::Has(options, Nan::New("gpio").ToLocalChecked()).ToChecked()) {
        Nan::MaybeLocal<v8::Value> maybe_gpio = Nan::Get(options, Nan::New("gpio").ToLocalChecked());
        v8::Local<v8::Value> gpio;
        if (!maybe_gpio.IsEmpty() && maybe_gpio.ToLocal(&gpio))
            config.ws281x.channel[0].gpionum = Nan::To<int>(gpio).FromMaybe(config.ws281x.channel[0].gpionum);
    }

    // brightness
    if (Nan::Has(options, Nan::New("brightness").ToLocalChecked()).ToChecked()) {
        Nan::MaybeLocal<v8::Value> maybe_brightness = Nan::Get(options, Nan::New("brightness").ToLocalChecked());
        v8::Local<v8::Value> brightness;
        if (maybe_brightness.ToLocal(&brightness))
            config.ws281x.channel[0].brightness = Nan::To<int>(brightness).FromMaybe(config.ws281x.channel[0].brightness);
    }

    // rawRGBW
    if (Nan::Has(options, Nan::New("rawRGBW").ToLocalChecked()).ToChecked()) {
        Nan::MaybeLocal<v8::Value> maybe_rawRGBW = Nan::Get(options, Nan::New("rawRGBW").ToLocalChecked());
        v8::Local<v8::Value> rawRGBW;
        if (maybe_rawRGBW.ToLocal(&rawRGBW))
            config.rawRGBW = Nan::To<int>(rawRGBW).FromMaybe(config.rawRGBW);
    }

    // stripType
    {
        Nan::MaybeLocal<v8::Value> maybe_stripType;
        v8::Local<v8::Value> stripType;

        if (Nan::Has(options, Nan::New("stripType").ToLocalChecked()).ToChecked())
            maybe_stripType = Nan::Get(options, Nan::New("stripType").ToLocalChecked());

        if (maybe_stripType.ToLocal(&stripType)) {
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
        } else {
            config.ws281x.channel[0].strip_type = WS2811_STRIP_RGB;
        }
    }

    // gamma (Uint8Array(256) expected) — disabled in rawRGBW
    if (!config.rawRGBW) {
        if (Nan::Has(options, Nan::New("gamma").ToLocalChecked()).ToChecked()) {
            Nan::MaybeLocal<v8::Value> maybe_gamma = Nan::Get(options, Nan::New("gamma").ToLocalChecked());
            v8::Local<v8::Value> gamma;

            if (maybe_gamma.ToLocal(&gamma)) {
                static uint8_t gammaCorrection[256];

                if (!gamma->IsUint8Array())
                    return Nan::ThrowError("ws281x.configure() - gamma must be a Uint8Array.");

                v8::Local<v8::Uint8Array> g = gamma.As<v8::Uint8Array>();
                if (g->Length() != 256)
                    return Nan::ThrowError("ws281x.configure() - gamma table "
                                           "must have length 256.");

                auto ab = g->Buffer();
                auto bs = ab->GetBackingStore();
                uint8_t *data = static_cast<uint8_t *>(bs->Data()) + g->ByteOffset();

                std::memcpy(gammaCorrection, data, 256);
                config.ws281x.channel[0].gamma = gammaCorrection;
            }
        }
    }

    // colorTemperature — disabled in rawRGBW
    if (!config.rawRGBW) {
        if (Nan::Has(options, Nan::New("colorTemperature").ToLocalChecked()).ToChecked()) {
            Nan::MaybeLocal<v8::Value> maybe_colorTemperature = Nan::Get(options, Nan::New("colorTemperature").ToLocalChecked());
            v8::Local<v8::Value> colorTemperature;
            if (maybe_colorTemperature.ToLocal(&colorTemperature))
                config.colorTemperature = Nan::To<int>(colorTemperature).FromMaybe(config.colorTemperature);
        }
    }

    if (config.ws281x.channel[0].count <= 0)
        return Nan::ThrowError("ws281x.configure() - 'leds' must be > 0.");

    // Initialize driver
    ws2811_return_t result = ws2811_init(&config.ws281x);

    if (result) {
        std::string msg = "ws281x.configure() - ws2811_init() failed: ";
        msg += ws2811_get_return_t_str(result);
        return Nan::ThrowError(msg.c_str());
    }

    if (!config.ws281x.channel[0].leds)
        return Nan::ThrowError("ws281x.configure() - ws2811_init succeeded but leds buffer is null.");

    config.initialized = true;
    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(Addon::reset) {
    Nan::HandleScope();

    if (config.initialized) {
        if (config.ws281x.channel[0].leds && config.ws281x.channel[0].count > 0) {
            std::memset(config.ws281x.channel[0].leds, 0, sizeof(uint32_t) * config.ws281x.channel[0].count);
            ws2811_render(&config.ws281x);
        }
        ws2811_fini(&config.ws281x);
    }

    // Clear config
    config = config_t{};
    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(Addon::render) {
    Nan::HandleScope();

    if (!config.initialized)
        return Nan::ThrowError("ws281x not configured.");

    if (info.Length() != 1)
        return Nan::ThrowError("ws281x.render() requires a Uint32Array of pixels.");

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

    // Pointer to JS backing store
    std::shared_ptr<v8::BackingStore> backing = arr->Buffer()->GetBackingStore();
    uint8_t *base = static_cast<uint8_t *>(backing->Data());
    uint32_t *data = reinterpret_cast<uint32_t *>(base + arr->ByteOffset());

    // Copy pixels
    std::memcpy(channel.leds, data, led_count * sizeof(uint32_t));

    if (!config.rawRGBW || !isRGBW()) {
        adjustColorTemperature(channel.leds, static_cast<int>(led_count));
        convertToRGBW(channel.leds, static_cast<int>(led_count));
    }

    ws2811_return_t rc = ws2811_render(&config.ws281x);

    if (rc) {
        std::ostringstream err;
        err << "ws281x.render() - ws2811_render failed: " << ws2811_get_return_t_str(rc);
        Nan::ThrowError(err.str().c_str());
        return;
    }

    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(Addon::sleep) {
    Nan::HandleScope();

    if (info.Length() < 1 || !info[0]->IsInt32())
        return Nan::ThrowTypeError("ws281x.sleep(ms) requires an integer milliseconds.");

    int32_t ms = info[0]->Int32Value(Nan::GetCurrentContext()).FromMaybe(0);
    if (ms < 0)
        ms = 0;

    usleep(ms * 1000);
    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_MODULE_INIT(initAddon) {
    Nan::SetMethod(target, "configure", Addon::configure);
    Nan::SetMethod(target, "render", Addon::render);
    Nan::SetMethod(target, "reset", Addon::reset);
    Nan::SetMethod(target, "sleep", Addon::sleep);
}

NODE_MODULE(addon, initAddon)
