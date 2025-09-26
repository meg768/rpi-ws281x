var ws281x = require('../index.js');

class Example {

    constructor() {
        this.leds = 169;
        
        // Configure ws281x
        ws281x.configure({leds:this.leds, stripType : 'grb', dma:10, gpio:18});

    }

    run() {
        let pixels = new Uint32Array(this.leds);

        // Create a fill color with red/green/blue.
        var red = 0, green = 255, blue = 0;
        var color = (red << 16) | (green << 8)| blue;

        for (var i = 0; i < this.leds; i++)
            pixels[i] = color;

        // Render to strip
        ws281x.render(pixels);
    }
    
};

var example = new Example();
example.run();
