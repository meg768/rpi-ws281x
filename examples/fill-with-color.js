var ws281x = require('../index.js');

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
        this.config.stripType = 'grbw';

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
