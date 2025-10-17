var ws281x = require('../index.js');

/*

// One time initialization, assumes an 8 pixel strip
ws281x.configure({ leds: 8, stripType:'grbw'});

// Create my pixels
var pixels = new Uint32Array(8);

// Set first three pixel pixels to red, green and blue
pixels[0] = 0xff000000;

// Render pixels to the Neopixel strip
ws281x.render(pixels);

*/


ws281x.configure({ leds: 8, stripType: 'grbw', xtransitions: ['RGBtoRGBW'] });

// Create my pixels
var pixels = new Uint32Array(8);

pixels[0] = 0x00ff0000; // röd
pixels[1] = 0x0000ff00; // grön
pixels[2] = 0x000000ff; // blå
ws281x.render(pixels);  // Röd/grön/blå ska fortfarande se "röd/grön/blå" ut (ev. lite vitmix)