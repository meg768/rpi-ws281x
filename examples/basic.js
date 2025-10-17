var ws281x = require('../index.js');

// One time initialization, assumes an 8 pixel strip
//ws281x.configure({ leds: 8, stripType: 'grbw' });
ws281x.configure({ leds: 8, stripType: 'grbw', transitions:'RGBtoRGBW' });

// Create my pixels
var pixels = new Uint32Array(8);

// Set first three pixel pixels to red, green and blue
pixels[0] = 0xff0000;
pixels[1] = 0x00ff00;
pixels[2] = 0x0000ff;

// Render pixels to the Neopixel strip
ws281x.render(pixels);
