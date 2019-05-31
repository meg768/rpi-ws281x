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

### Running Pixel

````javascript

var ws281x = require('rpi-ws281x');

class App {

    constructor() {
        // The number of leds in my strip
        this.leds = 169;
    }

    run() {
        // Configure ws281x
        ws281x.configure({leds:this.leds});

        // Create an pixel array matching the number of leds
        var pixels = new Uint32Array(this.leds);

        // Create a fill color (R/G/B)
        var color = (255 << 16) | (0 << 8)| 0;

        for (var i = 0; i < this.leds; i++)
            pixels[i] = color;

        ws281x.render(pixels);
    }
    
};

var app = new App();
app.run();


````
