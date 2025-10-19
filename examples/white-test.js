var ws281x = require('../index.js');

class Example {
    constructor() {
		this.config = {};

		// Number of leds in my strip
		this.config.leds = 169;

		// Use DMA 10 (default 10)
		this.config.dma = 10;

		// Set full brightness, a value from 0 to 255 (default 255)
		this.config.brightness = 128;

		// Set the GPIO number to communicate with the Neopixel strip (default 18)
		this.config.gpio = 18;

		// Specify RGB sequence
		this.config.stripType = 'grbw';

		// Set color temperature (using Kelvin)
		this.config.colorTemperature = 10000;

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
            pixels[i] = 0xFFFFFF;
        }
        // Render to strip
        ws281x.render(pixels);
    }
}

var example = new Example();
example.run();
