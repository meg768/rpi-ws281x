# rpi-ws281x

This is a npm module for connecting a Raspberyy Pi to Neopixel strips. It uses the 
library from **jgarff** https://github.com/jgarff/rpi_ws281x.

You may also check out another npm module https://www.npmjs.com/package/rpi-ws281x-native.

## Installation

````bash
$ npm install rpi-ws281x --save
````


## Usage

	var ws281x = require('rpi-ws281x');
    ws281x.configure({leds:100});

## Methods

- **configure(option)** -	Configures the ws281x strip. Must be called before anything else.
	* **leds**          -	Specifies the number of leds. Either use **leds** or **width** and **height**.
	* **width**         -	Specifies the width.
	* **height**        -	Specifies the height.
	* **map**	        -	If **width** and **height** is specified, it may used as pixel mapping. 
							Must be a Uint32Array or a string.
							Predefined maps are **matrix** (default) or **alternating-matrix**.
	* **gpio**          -	Specifies the GPIO number used. Default is number 18.
	* **strip**         -   Speifies the RGB sequence of the strip. Valid values
							are **rgb**, **brg**, **bgr**... etc. RGBW is not currenty supported.  
	* **brightness**    -	Brightness of pixels. Number from 0 to 255. Default 255.
- **render(pixels)**    -	Renders the pixels specified to the strip. The **pixels** parameter must be a Uint32Array
                        	and the same size as the number of leds specified.
- **reset()**           -	Resets configuration. 


## Examples

### Filling the Neopixel strip with a color

````javascript

var ws281x = require('rpi-ws281x');

class Example {

    run() {
        // Number of leds in my strip
        var leds = 169;

        // Configure ws281x
        ws281x.configure({leds:leds});

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
