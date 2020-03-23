var path = require("path");
var addon = require(path.join(__dirname, "build", "Release", "rpi-ws281x.node"));

class Module {
    constructor() {
        this.map  = undefined;
        this.leds = undefined;
    }

    configure(options) {
        var {width, height, map, leds, ...options} = options;
        
        if (width != undefined || height != undefined) {

            if (width == undefined) {
                throw new Error('Must specify width if height is specified.');
            }

            if (height == undefined) {
                throw new Error('Must specify height if width is specified.');
            }

            if (leds != undefined) {
                throw new Error('Please do not specify leds when both width and height are specified.');
            }

            leds = width * height;

            if (typeof map == 'string') {
                if (map == 'matrix') {
                    map = new Uint32Array(width * height);
        
                    for (var i = 0; i < map.length; i++) 
                        map[i] = i;
        
                }
                else if (map == 'alternating-matrix') {
                    map = new Uint32Array(width * height);
        
                    for (var i = 0; i < map.length; i++) {
                        var row = Math.floor(i / width), col = i % width;
                
                        if ((row % 2) === 0) {
                            map[i] = i;
                        }
                        else {
                            map[i] = (row+1) * width - (col+1);
                        }
                    }        
                }
            }
        }

        // Make sure the number of leds are specified
        if (leds == undefined) {
            throw new Error('Number of leds must be defined. Either by leds or by width and height.');
        }
    
        // If no map specified, create a default one...
        if (map == undefined) {
            map = new Uint32Array(leds);

            for (var i = 0; i < leds; i++)
                map[i] = i;
        }
        
        // Make sure we have a correct instance of pixel mapping
        if (!(map instanceof Uint32Array))
            throw new Error('Pixel mapping must be an Uint32Array.');

        if (map.length != leds) 
            throw new Error('Pixel mapping array must be of the same size as the number of leds.');

        this.map  = map;
        this.leds = leds;

        addon.configure({...options, leds:leds});
    }

    reset() {
        if (this.leds != undefined) {
            this.render(new Uint32Array(this.leds));
            addon.reset();    
        }
    }

    sleep(ms) {
        addon.sleep(ms);
    }

    render(pixels) {
        if (this.map != undefined) {
            // Convert to Uint32Array if a Buffer
            if (pixels instanceof Buffer)
                pixels = new Uint32Array(pixels.buffer, pixels.byteOffset);
 
            if (this.leds != pixels.length)
                throw new Error('Pixels must be of same length as number of leds in render().');

            addon.render(pixels, this.map);
        }
    }
}


module.exports = new Module();
