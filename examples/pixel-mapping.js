var ws281x = require('../index.js');

class Example {

    constructor() {
        // Current pixel position
        this.offset = 0;

        // Set my Neopixel configuration
        this.config = {};

        // By setting width and height instead of number of leds
        // you may use named pixel mappings.
        // Currently "matrix" and "alternating-matrix" are
        // supported. You may also set the "map" property
        // to a custom Uint32Array to define your own map.
        this.config.width = 13;
        this.config.height = 13;
        this.config.map = 'alternating-matrix';

        // Configure ws281x
        ws281x.configure(this.config);
    }

    loop() {
        var leds = this.config.width * this.config.height;
        var pixels = new Uint32Array(leds);

        // Set a specific pixel
        pixels[this.offset] = 0xFF0000;

        // Move on to next
        this.offset = (this.offset + 1) % leds;

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