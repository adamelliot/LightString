#include "LSLPTwinkle.h"

void LSLPTwinkle::setupMode(uint8_t mode) {
	size = random(4) + 2;
	variableSize = false;
	fadeRate = 0.93f;

	switch (mode) {
		case 0:
		colorCycle = true;
		size = 1;
		break;
		
		case 1: // Same color
		colorCycle = false;
		size = 1;
		break;
		
		case 2:
		colorCycle = true;
		break;
		
		case 3:
		colorCycle = false;
		break;
		
		case 4: // Looks like fire
		colorCycle = false;
		variableSize = true;
		break;
		
		case 5: // Static mode
		fadeRate = 0.875;
		size = 1;
		break;
		
		case 6: // Custom size for lights
		size = 16;
		colorCycle = true;
		fadeRate = 0.97f;
		break;
		
		case 7:
		size = 16;
		colorCycle = false;
		fadeRate = 0.97f;
		break;
	}
	
	colorIndex = 0;
	indexStep = random(6) + 3;

	col = CRGB::White;
	white = false;//random(7) > 5;
}

void LSLPTwinkle::nudge(int32_t data) {
	size++;
	if (size > 12) size = 1;
}

void LSLPTwinkle::update(uint32_t ms) {
	pixelBuffer->fade(fadeRate);

	if (variableSize)
		size = random(5) + 1;

	uint16_t len = pixelBuffer->getLength() / size;
	uint8_t newPoints = random(len >> (fadeRate > 0.9f ? 3 : 2)) + 1;

	if (colorCycle) {
		for (int i = 0; i < newPoints; i++) {
			uint16_t index = random(len) * size;
			for (int j = 0; j < size; j++) {
				CRGB _col = Palettes.getColor(colorIndex += indexStep);

				pixelBuffer->setPixel(index + j, _col);
			}
		}
	} else {
		CRGB _col;
		
		if (white) {
			_col = col;
		} else {
			int s = random(0xff);
			for (int i = s; i < s + 0xff; i++) {
				_col = Palettes.getColor(i % 0x100);
				if (_col.r + _col.g + _col.b > 64)
					break;
			}
		}
		
		for (int i = 0; i < newPoints; i++) {
			uint16_t index = random(len) * size;
			for (int j = 0; j < size; j++) {
				pixelBuffer->setPixel(index + j, _col);
			}
		}
	}

	LSLightProgram::update(ms);
}
