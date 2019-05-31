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

        // Set the GPIO number to communicate with the Neopixel strip
        this.config.gpio = 18;

        // The RGB sequence may vary on some strips. Valid values
        // are "rgb", "rbg", "grb", "gbr", "bgr", "brg" 
        this.config.strip = 'gbr';

        // Configure ws281x
        ws281x.configure(this.config);
    }

    run() {
        // Create an pixel array matching the number of leds
        var pixels = new Uint32Array(this.config.leds);

        // Create a fill color (R/G/B)
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
