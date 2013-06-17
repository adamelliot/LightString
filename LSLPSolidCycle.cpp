#include "LSLPSolidCycle.h"

LSLightProgram *factorySolidCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPSolidCycle(pixelBuffer, colorPalette, colorFunc);
}

void LSLPSolidCycle::setupMode(uint8_t mode) {
	switch (mode) {
		case 0:
		split = false;
		reverse = false;
		break;

		case 1: // Split
		split = true;
		offset = random(0x7f - 40) + 40;
		break;
		
		case 2: // Split Reverse
		split = true;
		reverse = true;
		break;
	}
	
	colorIndex = random(0xff);
	changeRate = random(4) + 3;
}

void LSLPSolidCycle::update() {
	colorIndex += changeRate;
	color_t col = colorPalette->getColor(colorIndex);
	uint16_t len = pixelBuffer->getLength();

	if (!split) {
		for (int i = 0; i < len; i++) {
			pixelBuffer->setPixel(i, col);
		}
	} else {
		uint8_t offset = reverse ? (len - offset - 1) : this->offset;
		color_t col2 = colorPalette->getColor((colorIndex + offset) % 0xff);

		for (int i = 0; i < (len >> 1); i++)
			pixelBuffer->setPixel(i, col);

		for (int i = (len >> 1); i < len; i++)
			pixelBuffer->setPixel(i, col2);
	}

	LSLightProgram::update();
}
