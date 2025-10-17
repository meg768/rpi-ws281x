# rpi-ws281x

A lightweight Node.js wrapper for controlling **WS281x / NeoPixel** LED strips using a **Raspberry Pi**. Built on top of the C library from [jgarff/rpi_ws281x](https://github.com/jgarff/rpi_ws281x).

## Installation

```bash
$ npm install rpi-ws281x --save
```

## Usage

```javascript

var ws281x = require('rpi-ws281x');

// One time initialization, assumes an 8 pixel strip
ws281x.configure({ leds: 8 });

// Create my pixels
var pixels = new Uint32Array(8);

// Set first three pixels to red, green and blue
// Colors are packed as 0xWWRRGGBB or 0x00RRGGBB (if RGB only)

pixels[0] = 0x00FF0000;
pixels[1] = 0x0000FF00;
pixels[2] = 0x000000FF;

// Render pixels to the Neopixel strip
ws281x.render(pixels);

```

## Methods

This module is simple and only has three methods **configure()**, **render()** and **reset()**.

### Configure

**`configure(options)`** — Configures the ws281x strip. Must be called once and before anything else.

The **options** parameter may have the following properties:

| Name          | Description |
| :-------------| :-----------|
| leds          | Number of pixels in the strip. |
| width, height | Instead of specifying the number of leds you may specify **width** and **height**. This assumes leds are ordered in a matrix shape. |
| gamma         | Gamma number for correction (default none) otherwise 2.2 is recommended. It may also specify an **UInt32Array** for custom gamma mapping. Length of array must be 256. |
| map           | An **UInt32Array** specifying a pixel mapping. If **width** and **height** is specified it may be a string for predefined mappings. Currently only `"serpentine"` is supported (`"alternating-matrix"` also works for backward compatibility). |
| dma           | Default **10**. |
| brightness    | Default **255**. |
| gpio          | Default **18**. |
| stripType     | Valid values are `"rgb"`, `"grb"`, `"brg"`, `"bgr"`, `"gbr"`. If using an RGBW-strip, just add `"w"` to the name. Default `"rgb"`. |
| transitions   | *(string, experimental)* One or more space-separated transition filters. See table below. |

#### Available transitions

Transitions is a series of filters before rendering pixels. Each transition converts RGB or RGBW values to new values ready for next transition.

| Name | Description |
| :--- | :----------- |
| warm-white | Converts RGB values to warm color tones. Input  `0x00RRGGBB` . Output `0x00RRGGBB`. |
| monochrome | Discards color and keeps brightness only. Input  `0x00RRGGBB` . Output `0x00RRGGBB`. |
| RGBtoRGBW | Converts RGB to RGBW by extracting the white channel using `min(R,G,B)`. Input  `0x00RRGGBB` . Output   `0xWWRRGGBB` . |

**Example:**

```js
ws281x.configure({
  leds: 64,
  transitions: "warm-white RGBtoRGBW"
});
```
### Render

**render(pixels)** - Renders pixels specified to the strip.

| Parameter | Description                                                  |
| :-------- | :----------------------------------------------------------- |
| pixels    | The **pixels** parameter must be an **Uint32Array** representing the color values of all pixels and the same size as the number of leds specified when configuring. |

### Reset

**reset()** - Resets configuration and turns all pixels off.

It's good practice to call `reset()` before exiting your program to ensure all LEDs are turned off cleanly.

## Examples

### Filling the Neopixel strip with a color

```javascript

var ws281x = require('rpi-ws281x');

class Example {
    constructor() {
        this.config = {};

        // Number of leds in my strip
        this.config.leds = 169;

        // Use DMA 10 (default 10)
        this.config.dma = 10;

        // Set full brightness, a value from 0 to 255 (default 255)
        this.config.brightness = 255;

        // Set the GPIO number to communicate with the Neopixel strip (default 18)
        this.config.gpio = 18;

        // Specify RGB sequence
        this.config.stripType = 'grb';

        // Set gamma correction to 2.2 
        this.config.gamma = 2.2;

        // Configure ws281x
        ws281x.configure(this.config);
    }

    run() {
        // Create a pixel array matching the number of leds.
        // This must be an instance of Uint32Array.
        let pixels = new Uint32Array(this.config.leds);
        let leds = this.config.leds;

        // Set pixels to a gradient from black to red
        for (let i = 0; i < leds; i++) {
            let red = (i / leds) * 255;
            let green = 0;
            let blue = 0;
            let color = (red << 16) | (green << 8) | blue;
            pixels[i] = color;
        }
        // Render to strip
        ws281x.render(pixels);
    }
}

var example = new Example();
example.run();

```

### Walking a pixel through the strip

```javascript

var ws281x = require('rpi-ws281x');

class Example {

    constructor() {
        // Current pixel position
        this.offset = 0;

        // Set my Neopixel configuration
        this.config = {leds:169, stripType : 'grb', gpio:18};

        // Configure ws281x
        ws281x.configure(this.config);
    }

    loop() {
        var pixels = new Uint32Array(this.config.leds);

        // Set a specific pixel
        pixels[this.offset] = 0xFF0000;

        // Move on to next
        this.offset = (this.offset + 1) % this.config.leds;

        // Render to strip
        ws281x.render(pixels);
    }

    run() {
        // Loop every 100 ms
        setInterval(this.loop.bind(this), 100);
    }
};

var example = new Example();
example.run();
```

### Walking a pixel with pixel mapping

```javascript

var ws281x = require('rpi-ws281x');

class Example {

    constructor() {
        // Current pixel position
        this.offset = 0;

        // Set my Neopixel configuration
        // By setting width and height instead of number of leds
        // you may use named pixel mappings.
        // Serpentine is a common mapping for matrices.
        this.config = { width: 13, height: 13, stripType: 'grb', gpio: 18, dma: 10, map: 'serpentine' };

        // Configure ws281x
        ws281x.configure(this.config);
    }

    loop() {
        var leds = this.config.width * this.config.height;
        var pixels = new Uint32Array(leds);

        // Set a specific pixel
        pixels[this.offset] = 0xFFFFFF;

        // Move on to next
        this.offset = (this.offset + 1) % leds;

        // Render to strip
        ws281x.render(pixels);
    }

    run() {
        // Loop every 50 ms
        setInterval(this.loop.bind(this), 50);
    }
};

var example = new Example();
example.run();
```

## Notes and Compatibility

> ⚠️ **Important:** Disable onboard audio on Raspberry Pi Zero / Zero 2 W to avoid DMA conflicts.  Run `sudo raspi-config`, then navigate to **Advanced Options → Audio → Disable**.

Tested on Raspberry Pi Zero W and Zero 2 W with Node.js versions **16–20** and WS2811/WS2812B (RGB) strips. 

## See also
- [jgarff/rpi_ws281x](https://github.com/jgarff/rpi_ws281x) — Original C library.
- [Adafruit NeoPixel Guide](https://learn.adafruit.com/adafruit-neopixel-uberguide/overview)

## Changelog

**2025-10-17** — Fixed bugs using *transitions* for RGBW-stips.   
**2025-10-12** — Added *transitions* in `configure()`.  
**2025-10-07** — Updated documentation and examples.  
**2025-10-06** — Added gamma correction via jgarff’s C-library and support for RGBW strips.

## License
MIT — feel free to use, fork and improve.

