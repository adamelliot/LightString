#include "LSLPHeartBeat.h"

LSLightProgram *factoryHeartBeat(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPHeartBeat(pixelBuffer, colorPalette, colorFunc);
}

void LSLPHeartBeat::setupMode(uint8_t mode) {
	lastTime = millis();

	double bpm = 72;

	msPerBeat = (60.0 / bpm) * 1000.0;
	msDubOffset = msPerBeat >> 2;

	nextLub = lastTime + msPerBeat;
	nextDub = nextLub + msDubOffset;

	colorIndex = random(0xff);
	changeRate = 1;
}

void LSLPHeartBeat::update(uint32_t ms) {
	uint32_t time = millis();

	pixelBuffer->fade(0.85f);

	if (time > nextLub) {
		color_t col = colorPalette->getColor(colorIndex += changeRate);

		pixelBuffer->clear(col);
		nextLub = time + msPerBeat;
	}

	if (time > nextDub) {
		color_t col = colorPalette->getColor(colorIndex += changeRate);

		pixelBuffer->clear(col);
		pixelBuffer->fade(0.75f);
		nextDub = nextLub + msDubOffset;
	}

	LSLightProgram::update(ms);
}
