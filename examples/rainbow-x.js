// demo-hue-swirls.js
const ws281x = require('../index.js');

const W = 13,
	H = 13;
const config = {
	width: W,
	height: H,
	stripType: 'grbw', // ditt val; lib:en ordnar byteordning
	gpio: 18,
	dma: 10,
	map: 'serpentine',
	Xtransitions: 'rgb-to-rgbw'
};

ws281x.configure(config);

// 0x00RRGGBB-packning (RGB-only). Libben sköter grb/rgw etc.
const packRGB = (r, g, b) => ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);

// Enkel HSV -> RGB (h: 0..1, s: 0..1, v: 0..1)
function hsvToRgb(h, s, v) {
	h = ((h % 1) + 1) % 1;
	const i = Math.floor(h * 6);
	const f = h * 6 - i;
	const p = Math.round(255 * v * (1 - s));
	const q = Math.round(255 * v * (1 - f * s));
	const t = Math.round(255 * v * (1 - (1 - f) * s));
	const vv = Math.round(255 * v);
	switch (i % 6) {
		case 0:
			return [vv, t, p];
		case 1:
			return [q, vv, p];
		case 2:
			return [p, vv, t];
		case 3:
			return [p, q, vv];
		case 4:
			return [t, p, vv];
		case 5:
			return [vv, p, q];
	}
}

// Förberäkna koordinater (centrerade) och polära värden för alla pixlar
const cx = (W - 1) / 2;
const cy = (H - 1) / 2;
const maxR = Math.hypot(cx, cy);

const cells = [];
for (let y = 0; y < H; y++) {
	for (let x = 0; x < W; x++) {
		const dx = x - cx;
		const dy = y - cy;
		const r = Math.hypot(dx, dy) / maxR; // 0..1
		const angle = Math.atan2(dy, dx); // -PI..PI
		const hueBase = angle / (2 * Math.PI) + 0.5; // 0..1
		cells.push({ x, y, r, hueBase });
	}
}

const pixels = new Uint32Array(W * H);

// Liten (valfri) gamma-kurva för mjukare lågintensitet
const makeGamma = (g = 2.2) => {
	const table = new Uint8Array(256);
	for (let i = 0; i < 256; i++) table[i] = Math.min(255, Math.round(255 * Math.pow(i / 255, 1 / g)));
	return table;
};
const gamma = makeGamma(2.2);
const gammaRGB = (r, g, b) => [gamma[r], gamma[g], gamma[b]];

// Animation: hue roterar över tid, radien pulserar för “andning”
let t = 0;
const FPS = 30;
const interval = Math.round(1000 / FPS);

function frame() {
	const hueSpeed = 0.03; // hur snabbt färghjulet roterar
	const pulseSpeed = 0.9; // hur snabbt radial pulsering sker
	const pulseAmt = 0.12; // hur mycket radien påverkar värdet

	for (let i = 0; i < cells.length; i++) {
		const { r, hueBase } = cells[i];

		// Hue = vinkel + global tidsförskjutning + lite radialt “twist”
		const hue = hueBase + t * hueSpeed + r * 0.08;

		// Saturation konstant hög; Value pulserar lätt med radien
		const value = 0.25 + 0.75 * (1 - pulseAmt + pulseAmt * Math.sin(t * pulseSpeed + r * Math.PI * 2) * 0.5 + 0.5);
		const [R, G, B] = hsvToRgb(hue, 1.0, Math.min(1.0, value));

		const [r2, g2, b2] = gammaRGB(R, G, B);
		pixels[i] = packRGB(r2, g2, b2);
	}

	ws281x.render(pixels);
	t += 1;
}

const timer = setInterval(frame, interval);

// Snyggt avslut: släck och återställ på Ctrl+C etc.
function cleanup() {
	clearInterval(timer);
	pixels.fill(0);
	ws281x.render(pixels);
	ws281x.reset();
	process.exit(0);
}
process.on('SIGINT', cleanup);
process.on('SIGTERM', cleanup);

// Kör första framen direkt
frame();
