var ws281x = require('../index.js');

let leds = 169;
let gamma = 1;

// Configure with gamma correction
ws281x.configure({ leds: leds, stripType: 'grb', gpio: 18, dma: 10, brightness: 255, gamma:gamma });

// Create my pixels
var pixels = new Uint32Array(leds);

// Set pixels to a gradient from black to red
for (let i = 0; i < leds; i++) {
    let red = (i/leds) * 255;
    let green = 0;
    let blue = 0;
    let color = (red << 16) | (green << 8) | blue;
    pixels[i] = color;
}   

// Render pixels to the Neopixel strip
ws281x.render(pixels);
