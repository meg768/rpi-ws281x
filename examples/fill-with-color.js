var ws281x = require('../index.js');

class Example {

    constructor() {
        this.config = {};

        // Number of leds in my strip
        this.config.leds = 169;

        // The RGB sequence may vary on some strips. Valid values
        // are "rgb", "rbg", "grb", "gbr", "bgr", "brg".
        // Default is "rgb".
        // RGBW strips are not currently supported.
        this.config.stripType = 'grb';

        // Configure ws281x
        ws281x.configure(this.config);

        console.log('Filling strip with color...');
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
        //console.log('Rendering...');
        ws281x.render(pixels);
    }
    
};

var example = new Example();
example.run();
