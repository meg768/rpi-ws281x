var path = require('path');
//var addon = require(path.join(__dirname, 'build', 'Release', 'rpi-ws281x.node'));
var addon = require('bindings')('rpi-ws281x.node');

class Module {
	constructor() {
		this.map = undefined;
		this.leds = undefined;

		function cleanup() {
			addon.reset();
			process.exit();
		}

		process.on('SIGUSR1', cleanup);
		process.on('SIGUSR2', cleanup);
		process.on('SIGINT', cleanup);
		process.on('SIGTERM', cleanup);
	}

	configure(options) {
		var { width, height, map, gamma, leds, ...options } = options;

		this.leds = undefined;
		this.map = undefined;
		this.gamma = gamma;

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

			if (map) {
				if (map instanceof Uint32Array) {
					if (map.length != leds) {
						throw new Error('Pixel mapping array must be of the same size as the number of leds.');
					}
					this.map = map;
				} else {
					if (typeof map == 'string') {
						if (map == 'alternating-matrix') {
							map = new Uint32Array(width * height);

							for (var i = 0; i < map.length; i++) {
								var row = Math.floor(i / width),
									col = i % width;

								if (row % 2 === 0) {
									map[i] = i;
								} else {
									map[i] = (row + 1) * width - (col + 1);
								}
							}

							this.map = map;
						}
					}
				}
			}
		}

		// Make sure the number of leds are specified
		if (leds == undefined) {
			throw new Error('Number of leds must be defined. Either by leds or by width and height.');
		}

		this.leds = leds;

		addon.configure({ ...options, leds: leds });
	}

	reset() {
		if (this.leds == undefined) {
			throw new Error('ws281x not configured.');
		}
		addon.reset();
	}

	sleep(ms) {
		addon.sleep(ms);
	}


	render(pixels) {
		let gammaCorrect = pixels => {
			let gamma = this.gamma;

			if (gamma == undefined) {
				return pixels;
			}

            const output = new Uint32Array(pixels.length);

			for (let i = 0; i < pixels.length; i++) {
				const rgb = pixels[i] >>> 0; // 0xRRGGBB

				const r1 = (rgb >>> 16) & 0xff;
				const g1 = (rgb >>> 8) & 0xff;
				const b1 = (rgb >>> 0) & 0xff;

				const r2 = Math.min(255, Math.round(Math.pow(r1 / 255, gamma) * 255));
				const g2 = Math.min(255, Math.round(Math.pow(g1 / 255, gamma) * 255));
				const b2 = Math.min(255, Math.round(Math.pow(b1 / 255, gamma) * 255));

				output[i] = ((r2 << 16) | (g2 << 8) | b2) >>> 0;
			}

			return output;
		};


        if (this.leds == undefined) {
			throw new Error('ws281x not configured.');
		}

		if (!(pixels instanceof Uint32Array)) {
			throw new Error('Pixels must be of type Uint32Array in render()');
		}

		if (this.leds != pixels.length) {
			throw new Error('Pixels must be of same length as number of leds in render()');
		}

		if (this.map && this.map instanceof Uint32Array) {
			var mapped = new Uint32Array(this.map.length);

			for (var i = 0; i < mapped.length; i++) {
				mapped[i] = pixels[this.map[i]];
			}

			addon.render(gammaCorrect(mapped));
		} else {
			addon.render(gammaCorrect(pixels));
		}
	}
}

module.exports = new Module();
