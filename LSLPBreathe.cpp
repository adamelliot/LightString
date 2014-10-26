#include "LSLPBreathe.h"

LSLightProgram *factoryBreathe(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette) {
	return new LSLPBreathe(pixelBuffer, colorPalette);
}

void LSLPBreathe::setupMode(uint8_t mode) {
	factor = 1.0f;
	min = 0.4f;
	step = 0.02f;
	direction = 1.0f;
}

void LSLPBreathe::update(uint32_t ms) {
	CRGB col = colorPalette->getColor(0);
	
	if (factor >= 1.0f) {
		direction = -1.0;
		factor = 1.0;
	} else if (factor <= min) {
		direction = 1.0;
		factor = min;
	}

	factor += (step * direction);
	if (factor > 1.0f) factor = 1.0f;

	col.r *= factor;
	col.g *= factor;
	col.b *= factor;
	
	pixelBuffer->clear(col);

	LSLightProgram::update(ms);
}
