var ws281x = require('../index.js');

class Example {

    constructor() {
		// Current pixel position
		this.offset = 0;

		// Set my Neopixel configuration
		// By setting width and height instead of number of leds
		// you may use named pixel mappings.
        // Serpentine is a common mapping for matrixes.
		this.config = { width: 13, height: 13, stripType: 'grb', gpio: 18, dma: 10, map: 'serpentine' };

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
        setInterval(this.loop.bind(this), 50);
    }
};

var example = new Example();
example.run();