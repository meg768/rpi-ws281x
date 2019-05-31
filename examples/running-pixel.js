var ws281x = require('../index.js');

class Example {

    constructor() {
        // The number of leds in my strip
        this.leds = 169;

        // Current pixel position
        this.offset = 0;

    }

    loop() {

        var pixels = new Uint32Array(this.leds);

        // Set a specific pixel
        pixels[this.offset] = 0xFF0000;

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

var example = new Example();
example.run();