const { type } = require('os');
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

		let isIntArray = a => {
			return a instanceof Uint8Array || a instanceof Uint16Array || a instanceof Uint32Array;
		};

		// Generate a gamma table for a specific gamma value
		let gammaTable = gamma => {
			const table = new Uint8Array(256);

			for (let i = 0; i < 256; i++) {
				table[i] = Math.min(255, Math.round(Math.pow(i / 255, gamma) * 255));
			}

			return table;
		};

		let convertToUint8Array = input => {
			if (input instanceof Uint8Array) {
				return input;
			}

			if (input instanceof Uint32Array) {
				return new Uint8Array(input);
			}

			if (input instanceof Uint16Array) {
				return new Uint8Array(input);
			}

			return null;
		};

		if (map instanceof Uint32Array) {
			if (map.length != leds) {
				throw new Error('Pixel mapping array must be of the same size as the number of leds.');
			}

			this.map = map;
		}

		if (width != undefined || height != undefined) {
			if (width == undefined || height == undefined) {
				throw new Error('Must specify both width and height.');
			}

			if (leds != undefined) {
				throw new Error('Please do not specify leds when both width and height are specified.');
			}

			leds = width * height;

			if (this.map == undefined && typeof map == 'string') {
				if (map == 'alternating-matrix' || map == 'serpentine') {
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

		// Make sure the number of leds are specified
		if (leds == undefined) {
			throw new Error('Number of leds must be defined. Either by leds or by width and height.');
		}

		// Handle gamma correction. Either a number or a an array with 256 entries.
		if (gamma != undefined) {
			if (typeof gamma === 'number') {
				if (gamma < 0) {
					throw new Error('Gamma must be a positive number.');
				}
				options.gamma = gammaTable(gamma);
			} else if (gamma instanceof Uint8Array || gamma instanceof Uint32Array || gamma instanceof Uint16Array) {
				if (gamma.length != 256) {
					throw new Error('Gamma table must have 256 entries.');
				}
				options.gamma = convertToUint8Array(gamma);
			}
		}

		// Final check that the gamma table is a Uint8Array
		if (options.gamma) {
			if (!(options.gamma instanceof Uint8Array)) {
				throw new Error('Gamma table must be a Uint8Array with 256 entries.');
			}
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

			addon.render(mapped);
		} else {
			addon.render(pixels);
		}
	}
}

module.exports = new Module();
