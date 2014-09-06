#include "LSLPStrobe.h"

LSLightProgram *factorySolidColor(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPSolidColor(pixelBuffer, colorPalette, colorFunc);
}

void LSLPSolidColor::setupMode(uint8_t mode) {
	colorIndex = 0;

	colors[0] = (color_t){255, 0, 127};
	colors[1] = (color_t){95, 0, 191};
	colors[2] = (color_t){255, 0, 0};
	colors[3] = (color_t){0, 255, 0};
	colors[4] = (color_t){0, 0, 255};
	colors[5] = (color_t){63, 191, 0};
	colors[6] = (color_t){191, 63, 0};
	colors[7] = (color_t){0, 255, 31};
	colors[8] = (color_t){0, 63, 255};
	colors[9] = (color_t){255, 255, 255};
}

void LSLPSolidColor::nudge(int32_t data) {
	colorIndex++;
	colorIndex %= MAX_COLORS;
}

void LSLPSolidColor::update(uint32_t ms) {
	color_t col = colors[colorIndex];
	pixelBuffer->clear(colorFunc(col.channels[0], col.channels[1], col.channels[2]));

	LSLightProgram::update(ms);
}
