# rpi-ws281x

This is a npm module for connecting a Raspbery Pi to Neopixel strips. It uses the 
library from **jgarff** https://github.com/jgarff/rpi_ws281x.

You may also check out another npm module https://www.npmjs.com/package/rpi-ws281x-native.

The module **rpi-ws281x-native** is excellent and does the job well. The reason I created
this npm module is that I had a problem with my hardware and had to start from scratch and
did not know where the problem was.

(It turned out that the solution was to use **dma** 10 instead of 5)

## Installation

````bash
$ npm install rpi-ws281x --save
````


## Usage

````javascript

var ws281x = require('rpi-ws281x');

// One time initialization
ws281x.configure({leds:16});

var pixels = new Uint32Array(16);

// Render pixels to the Neopixewl strip
ws281x.render(pixels);

````


## Methods

- **configure(options)**  -	Configures the ws281x strip. Must be called before anything else. See
							examples below.
- **render(pixels)**    -	Renders the pixels specified to the strip. The **pixels** parameter must 
							be an Uint32Array representing the color values of all pixels
                        	and the same size as the number of leds specified when configuring.
- **reset()**           -	Resets configuration. 


## Examples

### Filling the Neopixel strip with a color

````javascript

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

        // The RGB sequence may vary on some strips. Valid values
        // are "rgb", "rbg", "grb", "gbr", "bgr", "brg".
        // Default is "rgb".
        // RGBW strips are not currently supported.
        this.config.strip = 'grb';

        // Configure ws281x
        ws281x.configure(this.config);
    }

    run() {
        // Create a pixel array matching the number of leds.
        // This must be an instance of Uint32Array.
        var pixels = new Uint32Array(this.config.leds);

        // Create a fill color with red/green/blue.
        var red = 255, green = 0, blue = 0;
        var color = (red << 16) | (green << 8)| blue;

        for (var i = 0; i < this.config.leds; i++)
            pixels[i] = color;

        // Render to strip
        ws281x.render(pixels);
    }
    
};

var example = new Example();
example.run();

````
