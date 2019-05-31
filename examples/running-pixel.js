var ws281x = require('../index.js');

class App {

    constructor() {
        // The number of leds in my strip
        this.leds = 169;

        this.offset = 0;

        this.run();
    }

    loop() {

        // Clear all pixels
        for (var i = 0; i < this.leds; i++)
            this.pixels[i] = 0;

        // Set a specific pixel
        this.pixels[this.offset] = 0xFF0000;

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
