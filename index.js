var path = require("path");
var addon = require(path.join(__dirname, "build", "Release", "rpi-ws281x.node"));

class Module {
    constructor() {
    }

    configure(options) {
        addon.configure(options);
    }

    reset() {
        addon.reset();
    }

    sleep(ms) {
        addon.sleep(ms);
    }

    render(pixels) {
        addon.render(pixels);
    }
}


module.exports = new Module();
