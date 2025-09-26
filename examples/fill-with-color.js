var ws281x = require('../index.js');

class Example {

    constructor() {
        this.config = {};

        // Configure ws281x
        ws281x.configure(this.config);

    }

    run() {
        // Create a pixel array matching the number of leds.
        // This must be an instance of Uint32Array.
        var pixels = new Uint32Array(this.config.leds);

        // Create a fill color with red/green/blue.
        var red = 0, green = 32, blue = 0;
        var color = (red << 16) | (green << 8)| blue;

        /*for (var i = 0; i < this.config.leds; i++)
            this.pixels[i] = i % 2 == 0 ? 0 : color;
*/
        // Render to strip
        console.log('Rendering...');
        ws281x.render(this.pixels);
    }
    
};

var example = new Example();
example.run();
