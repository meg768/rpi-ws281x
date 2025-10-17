let ws281x = require('../index.js');

// Configure NeoPixels
// By setting width and height instead of number of leds
// you may use named pixel mappings.
// Serpentine is a common mapping for matrixes.
let config = { width: 13, height: 13, stripType: 'grb', gpio: 18, dma: 10, map: 'serpentine' };

// Create an array to hold the pixels
let pixels = new Uint32Array(config.width * config.height);

// Configure ws281x
ws281x.configure(config);

// Set pixels to a matrix with rainbow colors
pixels[0] = 0xff0000;

// Render to strip
ws281x.render(pixels);
