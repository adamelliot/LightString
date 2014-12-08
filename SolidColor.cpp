#include "SolidColor.h"

void SolidColor::setupMode(uint8_t mode) {
	colorIndex = 0;

	colors[0] = CRGB(255, 0, 127);
	colors[1] = CRGB(95, 0, 191);
	colors[2] = CRGB(255, 0, 0);
	colors[3] = CRGB(0, 255, 0);
	colors[4] = CRGB(0, 0, 255);
	colors[5] = CRGB(63, 191, 0);
	colors[6] = CRGB(191, 63, 0);
	colors[7] = CRGB(0, 255, 31);
	colors[8] = CRGB(0, 63, 255);
	colors[9] = CRGB(255, 255, 255);
}

void SolidColor::nudge(int32_t data) {
	colorIndex++;
	colorIndex %= MAX_COLORS;
}

void SolidColor::update(uint32_t ms) {
	CRGB col = colors[colorIndex];
	pixelBuffer->clear(col);

	LightProgram::update(ms);
}
