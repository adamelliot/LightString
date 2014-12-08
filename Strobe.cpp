#include "Strobe.h"

void Strobe::setupMode(uint8_t mode) {
	colorIndex = random(0xff);
	frames = 1;
	frameCount = 0;
}

void Strobe::nudge(int32_t data) {
	frames++;
	if (frames > 8) frames = 1;
}

void Strobe::update(uint32_t ms) {
	CRGB col = Palettes.getColor(colorIndex);

	frameCount++;
	frameCount %= (frames * 2);

	if ((frameCount / frames) == 1) {
		pixelBuffer->clear(col);
	} else {
		pixelBuffer->clear();
	}

	LightProgram::update(ms);
}
