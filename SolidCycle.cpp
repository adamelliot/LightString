#include "SolidCycle.h"

void SolidCycle::setupMode(uint8_t mode) {
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

void SolidCycle::update(uint32_t ms) {
	colorIndex += changeRate;
	CRGB col = Palettes.getColor(colorIndex);
	uint16_t len = pixelBuffer->getLength();

	if (!split) {
		for (int i = 0; i < len; i++) {
			pixelBuffer->setPixel(i, col);
		}
	} else {
		uint8_t offset = reverse ? (len - offset - 1) : this->offset;
		CRGB col2 = Palettes.getColor((colorIndex + offset) % 0xff);

		for (int i = 0; i < (len >> 1); i++)
			pixelBuffer->setPixel(i, col);

		for (int i = (len >> 1); i < len; i++)
			pixelBuffer->setPixel(i, col2);
	}

	LightProgram::update(ms);
}
