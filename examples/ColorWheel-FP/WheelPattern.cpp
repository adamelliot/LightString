#include "WheelPattern.h"

void WheelPattern::setupMode(uint8_t mode, LightString::PatternConfig *config) {
	counter = 0;
}

// Update is passed the amount of milliseconds since the last frame
void WheelPattern::update(uint32_t ms) {
	uint16_t len = pixelBuffer->getLength();
	float hueStep = (float)len / 360;
	counter++;

	for (uint16_t i = 0; i < len; i++) {
		(*pixelBuffer)[i] = HSVf(hueStep * (float)(i + counter), 1, 1);
	}

	// (*pixelBuffer)[0].println();
}
