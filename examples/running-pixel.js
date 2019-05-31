var ws211x = require('../index.js');


class App {

    constructor() {
        this.leds = 169;
        this.offset = 0;

        this.run();
    }

    loop() {

        for (var i = 0; i < this.leds; i++)
            this.pixels[i] = 0;

        this.pixels[this.offset] = 0xFF0000;
        this.offset = (this.offset + 1) % this.leds;

        ws211x.render(this.pixels);
    }

    run() {
        ws211x.configure({leds:this.leds});
        setInterval(this.loop.bind(this), 100);
    }
    
};

new App();
