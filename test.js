#!/usr/bin/env node


var ws211x = require('./index.js');


class App {


    constructor() {
        this.offset    = 0;
        this.width     = 13;
        this.height    = 13;
        this.leds      = this.width * this.height;
        this.pixels    = new Uint32Array(this.leds);
        this.map       = this.mapping();
        this.stripType = 'rgb'
    }

	mapping() {
		
		var map = new Uint16Array(this.width * this.height);

		for (var i = 0; i < map.length; i++) {
			var row = Math.floor(i / this.width), col = i % this.width;
	
			if ((row % 2) === 0) {
				map[i] = i;
			}
			else {
				map[i] = (row+1) * width - (col+1);
			}
		}

		return map;
	
	}

    loop() {

        for (var i = 0; i < this.leds; i++)
            pixels[i] = 0;

        pixels[this.offset] = 0xFFFFFF;

        ws211x.render(this.pixels, this.map);

        this.offset = (this.offset + 1) % (this.width * this.height);

    }

    run() {
        ws211x.configure({leds:169, map:this.map, stripType:this.stripType})
        setInterval(this.loop.bind(this), 100);
    }
    
};

var app = new App();
app.run();
