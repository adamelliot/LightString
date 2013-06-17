#include "LSLPSolidCycle.h"

LSLightProgram *factorySolidCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPSolidCycle(pixelBuffer, colorPalette, colorFunc);
}

void LSLPSolidCycle::setupMode(uint8_t mode) {
	colorIndex = random(0xff);
	changeRate = random(4) + 3;
}

void LSLPSolidCycle::update() {
	colorIndex += changeRate;
	color_t col = colorPalette->getColor(colorIndex);

	for (int i = 0; i < pixelBuffer->getLength(); i++)
		pixelBuffer->setPixel(i, col);

	LSLightProgram::update();
}
