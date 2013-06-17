#include "LSLPBreathe.h"

LSLightProgram *factoryBreathe(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPBreathe(pixelBuffer, colorPalette, colorFunc);
}

void LSLPBreathe::setupMode(uint8_t mode) {
	factor = 1.0f;
	min = 0.4f;
	step = 0.02f;
	direction = 1.0f;
}

void LSLPBreathe::update() {
	color_t col = colorPalette->getColor(0);
	
	if (factor >= 1.0f) {
		direction = -1.0;
		factor = 1.0;
	} else if (factor <= min) {
		direction = 1.0;
		factor = min;
	}

	factor += (step * direction);
	if (factor > 1.0f) factor = 1.0f;

	col.channels[0] *= factor;
	col.channels[1] *= factor;
	col.channels[2] *= factor;
	
	pixelBuffer->clear(col);

	LSLightProgram::update();
}
