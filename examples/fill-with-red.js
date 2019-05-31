var ws281x = require('../index.js');

class App {

    constructor() {
        // The number of leds in my strip
        this.leds = 169;
    }

    run() {
        // Configure ws281x
        ws281x.configure({leds:this.leds});

        var pixels = new Uint32Array(this.leds);
        
        for (var i = 0; i < this.leds; i++)
            pixels[i] = (255 << 16) | (0 << 8)| 0;

        ws281x.render(pixels);
    }
    
};

var app = new App();
app.run();
