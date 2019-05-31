var ws281x = require('../index.js');

class Example {

    constructor() {
        this.leds = 169;

        // Configure ws281x
        ws281x.configure({leds:this.leds});
    }

    run() {
        // Number of leds
        var leds = this.leds;

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
