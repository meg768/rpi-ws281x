# rpi-ws281x

Module for connecting to Neopixel strips.

## Installation

````bash
	$ npm install rpi-ws281x --save
````


## Usage

	var ws281x = require('rpi-ws281x');
    ws281x.configure({leds:100});


## Examples

### Filling the Neopixel strip with a color

````javascript

var ws281x = require('rpi-ws281x');

class Example {

    run() {
        // Number of leds
        var leds = 169;

        // Configure ws281x
        ws281x.configure({leds:leds});

        // Create an pixel array matching the number of leds
        var pixels = new Uint32Array(leds);

        // Create a fill color (R/G/B)
        var color = (255 << 16) | (0 << 8)| 0;

        for (var i = 0; i < leds; i++)
            pixels[i] = color;

        // Render to strip
        ws281x.render(pixels);
    }
    
};

var example = new Example();
example.run();



````
