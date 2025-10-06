var ws281x = require('../index.js');

let leds = 169;

ws281x.configure({ leds: leds, stripType: 'grb', gpio: 18, dma: 10, brightness: 255, gamma:2.2 });

// Create my pixels
var pixels = new Uint32Array(leds);

for (let i = 0; i < leds; i++) {
    let red = 256/leds * i;
    let green = 0;
    let blue = 0;
    let color = (red << 16) | (green << 8) | blue;
    pixels[i] = color;
}   


// Render pixels to the Neopixel strip
ws281x.render(pixels);
