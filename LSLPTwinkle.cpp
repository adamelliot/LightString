#include "LSLPTwinkle.h"

LSLightProgram *factoryTwinkle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPTwinkle(pixelBuffer, colorPalette, colorFunc);
}

void LSLPTwinkle::setupMode(uint8_t mode) {
	size = random(4) + 2;
	variableSize = false;
	fastFade = 0;
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
		fastFade = 1;
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

	col = (color_t){0xff, 0xff, 0xff};
	white = false;//random(7) > 5;
}

void LSLPTwinkle::nudge(int32_t data) {
	size++;
	if (size > 12) size = 1;
}

void LSLPTwinkle::update(uint32_t ms) {
	if (fastFade) {
		pixelBuffer->fade(fastFade);
	} else {
		pixelBuffer->fade(fadeRate);
	}

	if (variableSize)
		size = random(5) + 1;

	uint16_t len = pixelBuffer->getLength() / size;
	uint8_t newPoints = random(len >> (fastFade ? 1 : (fadeRate > 0.9f ? 3 : 2))) + 1;

	if (colorCycle) {
		for (int i = 0; i < newPoints; i++) {
			uint16_t index = random(len) * size;
			for (int j = 0; j < size; j++) {
				color_t _col = colorPalette->getColor(colorIndex += indexStep);

				pixelBuffer->setPixel(index + j, _col);
			}
		}
	} else {
		color_t _col;
		
		if (white) {
			_col = col;
		} else {
			int s = random(0xff);
			for (int i = s; i < s + 0xff; i++) {
				_col = colorPalette->getColor(i % 0x100);
				if (_col.channels[0] + _col.channels[1] + _col.channels[2] > 64)
					break;
			}
		}
		
		for (int i = 0; i < newPoints; i++) {
			uint16_t index = random(len) * size;
			for (int j = 0; j < size; j++) {
				/*colorFunc(col.channels[0], col.channels[1], col.channels[2])*/
				pixelBuffer->setPixel(index + j, _col);
			}
		}
	}

	LSLightProgram::update(ms);
}
