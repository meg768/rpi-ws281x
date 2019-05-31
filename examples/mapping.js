#!/usr/bin/env node


var ws211x = require('../index.js');


class App {


    constructor() {
        this.offset     = 0;
        this.width      = 13;
        this.height     = 13;
        this.brightness = 255;
        this.leds       = this.width * this.height;
        this.pixels     = new Uint32Array(this.width * this.height);
        this.map        = 'alternating-matrix';
        this.strip      = 'grb';
    }


    loop() {

        for (var i = 0; i < this.leds; i++)
            this.pixels[i] = 0;

        this.pixels[this.offset] = 0xFF0000;
        this.offset = (this.offset + 1) % this.leds;

        ws211x.render(this.pixels);
    }

    run() {
        console.log({brightness:this.brightness, width:this.width, height:this.height, map:this.map, strip:this.strip});
        ws211x.configure(options);
        setInterval(this.loop.bind(this), 100);
    }
    
};

var app = new App();
app.run();
