#include "addon.h"

#include <sstream>
#include <string>

#define DEFAULT_TARGET_FREQ 800000
#define DEFAULT_GPIO_PIN 18
#define DEFAULT_DMA 5
#define DEFAULT_TYPE ws281x_STRIP_RGB

static ws281x_t ws281x;

NAN_METHOD(Addon::configure)
{
    Nan::HandleScope();

    if (ws281x.freq != 0)
    {
        return Nan::ThrowError("ws281x already configured.");
    }

    memset(&ws281x, 0, sizeof(ws281x_t));

    ws281x.freq = DEFAULT_TARGET_FREQ;
    ws281x.dmanum = DEFAULT_DMA;

    ws281x.channel[0].gpionum = DEFAULT_GPIO_PIN;
    ws281x.channel[0].count = 0;
    ws281x.channel[0].invert = 0;
    ws281x.channel[0].brightness = 255;
    ws281x.channel[0].strip_type = DEFAULT_TYPE;
    ws281x.channel[0].gamma = NULL;

    ws281x.channel[1].gpionum = 0;
    ws281x.channel[1].count = 0;
    ws281x.channel[1].invert = 0;
    ws281x.channel[1].brightness = 0;
    ws281x.channel[1].strip_type = 0;

    if (info.Length() != 1)
    {
        return Nan::ThrowError("configure() requires an argument.");
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
            ws281x.channel[0].count = Nan::To<int>(leds).FromMaybe(ws281x.channel[0].count);
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
            ws281x.dmanum = Nan::To<int>(dma).FromMaybe(ws281x.dmanum);
    }

    ///////////////////////////////////////////////////////////////////////////
    // gpio
    if (Nan::Has(options, Nan::New<v8::String>("gpio").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_gpio = Nan::Get(options, Nan::New<v8::String>("gpio").ToLocalChecked());
        v8::Local<v8::Value> gpio;

        if (!maybe_gpio.IsEmpty() && maybe_gpio.ToLocal(&gpio))
        {
            ws281x.channel[0].gpionum = Nan::To<int>(gpio).FromMaybe(ws281x.channel[0].gpionum);
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // brightness
    if (Nan::Has(options, Nan::New<v8::String>("brightness").ToLocalChecked()).ToChecked())
    {
        Nan::MaybeLocal<v8::Value> maybe_brightness = Nan::Get(options, Nan::New<v8::String>("brightness").ToLocalChecked());
        v8::Local<v8::Value> brightness;

        if (maybe_brightness.ToLocal(&brightness))
            ws281x.channel[0].brightness = Nan::To<int>(brightness).FromMaybe(ws281x.channel[0].brightness);
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
                ws281x.channel[0].strip_type = ws281x_STRIP_RGB;
            }
            else if (stripTypeValue == "rbg")
            {
                ws281x.channel[0].strip_type = ws281x_STRIP_RBG;
            }
            else if (stripTypeValue == "grb")
            {
                ws281x.channel[0].strip_type = ws281x_STRIP_GRB;
            }
            else if (stripTypeValue == "gbr")
            {
                ws281x.channel[0].strip_type = ws281x_STRIP_GBR;
            }
            else if (stripTypeValue == "brg")
            {
                ws281x.channel[0].strip_type = ws281x_STRIP_BRG;
            }
            else if (stripTypeValue == "bgr")
            {
                ws281x.channel[0].strip_type = ws281x_STRIP_BGR;
            }
            else if (stripTypeValue == "grbw")
            {
                ws281x.channel[0].strip_type = SK6812_STRIP_GRBW;
            }
        }
        else
        {
            ws281x.channel[0].strip_type = ws281x_STRIP_RGB;
        }
    }

    if (ws281x.channel[0].count <= 0)
    {
        return Nan::ThrowError("configure(): 'leds' must be > 0.");
    }

    ws281x_return_t result = ws281x_init(&ws281x);

    if (result)
    {
        std::ostringstream errortext;
        errortext << "configure(): ws281x_init() failed: " << ws281x_get_return_t_str(result);
        return Nan::ThrowError(errortext.str().c_str());
    }

    if (!ws281x.channel[0].leds)
    {
        return Nan::ThrowError("configure(): ws281x_init succeeded but leds buffer is null.");
    }

    info.GetReturnValue().Set(Nan::Undefined());
};

NAN_METHOD(Addon::reset)
{
    Nan::HandleScope();

    if (ws281x.freq != 0)
    {
        if (ws281x.channel[0].leds && ws281x.channel[0].count > 0)
        {
            memset(ws281x.channel[0].leds, 0, sizeof(uint32_t) * ws281x.channel[0].count);
            ws281x_render(&ws281x);
        }
        ws281x_fini(&ws281x);
    }
    ws281x.freq = 0;
    info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(Addon::render)
{
    Nan::HandleScope();

    if (ws281x.freq == 0)
    {
        return Nan::ThrowError("render(): ws281x not configured.");
    }
    if (info.Length() != 1)
    {
        return Nan::ThrowError("render() requires pixels");
    }
    if (!info[0]->IsUint32Array())
    {
        return Nan::ThrowError("render() requires pixels to be a Uint32Array.");
    }

    // retrieve buffer from argument 1
    if (!node::Buffer::HasInstance(info[0]))
    {
        Nan::ThrowTypeError("render() expected pixels to be a Buffer");
        return;
    }

    ws281x_channel_t channel = ws281x.channel[0];

    v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
    auto buffer = info[0]->ToObject(context).ToLocalChecked();
    uint32_t *data = (uint32_t *)node::Buffer::Data(buffer);

    const int numBytes = std::min(node::Buffer::Length(buffer), sizeof(ws281x_led_t) * channel.count);

    memcpy(channel.leds, data, numBytes);

    ws281x_render(&ws281x);

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
