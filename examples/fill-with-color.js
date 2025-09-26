var ws281x = require('../index.js');

class Example {

    constructor() {
        this.leds = 169;
        this.pixels = new Uint32Array(this.leds);
        
        // Configure ws281x
        ws281x.configure({leds:this.leds, stripType : 'grb', gpio:18});

    }

    run() {

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
