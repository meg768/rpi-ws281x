# rpi-ws281x

This is a npm module for connecting a Raspbery Pi to Neopixel strips. It uses the 
library from **jgarff** https://github.com/jgarff/rpi_ws281x.

You may also check out another npm module https://www.npmjs.com/package/rpi-ws281x-native.

The module **rpi-ws281x-native** is excellent and does the job well. The reason I created
this npm module is that I had a problem with my hardware and had to start from scratch and
did not know where the problem was.

(It turned out that the solution was to use **dma** 10 instead of 5)

## Installation

````bash
$ npm install rpi-ws281x --save
````


## Usage

	var ws281x = require('rpi-ws281x');
	...
	// One time initialization
    ws281x.configure({leds:16});
	...
	var pixels = new Uint32Array(16);
	...
	// Render pixels to the Neopixewl strip
    ws281x.render(pixels);


## Methods

- **configure(options)**  -	Configures the ws281x strip. Must be called before anything else. See
							examples below.
- **render(pixels)**    -	Renders the pixels specified to the strip. The **pixels** parameter must 
							be an Uint32Array representing the color values of all pixels
                        	and the same size as the number of leds specified when configuring.
- **reset()**           -	Resets configuration. 


## Examples

### Filling the Neopixel strip with a color

````javascript

var ws281x = require('rpi-ws281x');

class Example {

    constructor() {
        // NUmber of leds in my strip
        this.leds = 169;

        // Configure ws281x
        ws281x.configure({leds:this.leds});
    }

    run() {
        // Number of leds
        var leds = this.leds;

        // Create an pixel array matching the number of leds
        var pixels = new Uint32Array(leds);

        // Create a fill color (R/G/B)
        var color = (255 << 16) | (0 << 8)| 0;

        for (var i = 0; i < leds; i++)
            pixels[i] = color;

        // Render to strip
        ws281x.render(pixels);
    }
    
};

var example = new Example();
example.run();

````
