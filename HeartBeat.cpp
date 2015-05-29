#include "HeartBeat.h"

void HeartBeat::setupMode(uint8_t mode) {
	lastTime = millis();

	double bpm = 72;

	msPerBeat = (60.0 / bpm) * 1000.0;
	msDubOffset = msPerBeat >> 2;

	nextLub = lastTime + msPerBeat;
	nextDub = nextLub + msDubOffset;

	colorIndex = random(0xff);
	changeRate = 1 + random(5);
}

void HeartBeat::update(uint32_t ms) {
	uint32_t time = millis();

	pixelBuffer->fade(227);

	if (time > nextLub) {
		CRGB col = Palettes.getColor(colorIndex += changeRate);

		pixelBuffer->showColor(col);
		nextLub = time + msPerBeat;
	}

	if (time > nextDub) {
		CRGB col = Palettes.getColor(colorIndex += changeRate);

		pixelBuffer->showColor(col);
		pixelBuffer->fade(192);
		nextDub = nextLub + msDubOffset;
	}

	LightProgram::update(ms);
}
