# rpi-ws281x

This is a npm module (wrapper) for connecting a Raspberry Pi to Neopixel strips. It uses the C-library from **jgarff** https://github.com/jgarff/rpi_ws281x.

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

// Set first three pixel pixels to red, green and blue
pixels[0] = 0xff0000;
pixels[1] = 0x00ff00;
pixels[2] = 0x0000ff;

// Render pixels to the Neopixel strip
ws281x.render(pixels);

```

## Methods

This module is simple and only has three methods **configure()**, **render()** and **reset()**.

### Configure

**configure(options)** - Configures the ws281x strip. Must be called once and before anything else. See examples below.

The **options** parameter may have the following properties.

| Name          | Description                                                  |
| :------------ | :----------------------------------------------------------- |
| leds          | Number of pixels in the strip.                               |
| width, height | Instead of specifying the number of leds you may specify **width** and **height**. This assumes leds are ordered in a matrix shape. |
| gamma         | Gamma number for correction (default none) otherwise 2.2 is recommended. It may also specify an **UInt32Array** for custom gamma mapping. Length of array must be 256. |
| map           | An **UInt32Array** specifying a pixel mapping. If **width** and **height** is specified it may be a string for predefined mappings. Currently only "serpentine" is supported ("alternating-matrix" also works for backward compatibility). |
| dma           | Default **10**.                                              |
| brightness    | Default **255**.                                             |
| gpio          | Default **18**.                                              |
| stripType     | Valid values are **"rgb"**, **"grb"**, **"brg"**, **"bgr"**, **"gbr"**. If using an RGBW-strip, just add "w" to the name. Default **"rgb"**. |
| transitions   | *(string, experimental)* Not yet implemented. Built-in pixel transitions. Valid values: **"none"** (default), **"white-shift"**. When set to **"white-shift"**, RGB input values (packed as `0x00RRGGBB`) are converted to WRGB (packed as `0xWWRRGGBB`) by extracting the white channel using `min(R,G,B)`. Only effective for RGBW-strips. Unknown values are ignored (treated as `"none"`). Maybe some day **"white-shift, warm-white, gamma-2.2"** will work. Still working on details. |

### Render

**render(pixels)** - Renders pixels specified to the strip.

| Parameter | Description                                                  |
| :-------- | :----------------------------------------------------------- |
| pixels    | The **pixels** parameter must be an **Uint32Array** representing the color values of all pixels and the same size as the number of leds specified when configuring. |

### Reset

**reset()** - Resets configuration and turns all pixels off.

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

## Notes

Be sure to turn off audio if using a Raspberry Pi Zero. Ask ChatGPT how to do this with your version of OS.



## Updates

- 2025-10-06 - Added gamma corrections using the jgarff C-library and added support for RGBW strips. 

- 2025-10-07 - Updated documentation and examples.

- 2025-10-12 - Added **transitions** in **configure()**.

  
