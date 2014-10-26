#include "LSLPStrobe.h"

LSLightProgram *factoryStrobe(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette) {
	return new LSLPStrobe(pixelBuffer, colorPalette);
}

void LSLPStrobe::setupMode(uint8_t mode) {
	colorIndex = random(0xff);
	frames = 1;
	frameCount = 0;
}

void LSLPStrobe::nudge(int32_t data) {
	frames++;
	if (frames > 8) frames = 1;
}

void LSLPStrobe::update(uint32_t ms) {
	CRGB col = colorPalette->getColor(colorIndex);

	frameCount++;
	frameCount %= (frames * 2);

	if ((frameCount / frames) == 1) {
		pixelBuffer->clear(col);
	} else {
		pixelBuffer->clear();
	}

	LSLightProgram::update(ms);
}
