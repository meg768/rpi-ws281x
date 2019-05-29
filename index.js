var path = require("path");
var addon = require(path.join(__dirname, "build", "Release", "rpi-ws281x.node"));

class Module {
    constructor() {
        this.map = undefined;
        this.leds = 0;
    }

    configure(options) {
        var {map, leds} = options;

        if (leds == undefined) {
            throw new Error('Number of leds must be defined.');
        }
    
        if (map == undefined) {
            map = new Uint32Array(leds);

            for (var i = 0; i < leds; i++)
                map[i] = i;
        }
        
        if (!(map instanceof Uint32Array))
            throw new Error('Pixel mapping must be an Uint32Array.');

        this.map = map;
        this.leds = leds;

        addon.configure(options);
    }

    reset() {
        this.render(new Uint32Array(this.leds));
        addon.reset();
    }

    sleep(ms) {
        addon.sleep(ms);
    }

    render(pixels) {
        addon.render(pixels, this.map);
    }
}


module.exports = new Module();
