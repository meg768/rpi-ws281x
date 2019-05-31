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

		// Create an array of pixels
		this.pixels = new Uint32Array(this.leds);

		// Reset cursor
		this.offset = 0;

		this.run();
    }

    loop() {

        // Clear all pixels
        for (var i = 0; i < this.leds; i++)
            this.pixels[i] = 0;

        // Set a specific pixel
        this.pixels[this.offset] = 0xFF000F;

        // Move on to next
        this.offset = (this.offset + 1) % this.leds;

        // Render to strip
        ws281x.render(this.pixels);
    }

    run() {
        // Configure ws281x
        ws281x.configure({leds:this.leds});

        // Loop every 100 ms
        setInterval(this.loop.bind(this), 100);
    }
    
};

new App();

````
