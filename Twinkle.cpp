#include "Twinkle.h"

void Twinkle::setupMode(uint8_t mode) {
	size = random(2) + 1;
	variableSize = false;
	fadeRate = 237;

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
		fadeRate = 0.80;
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
  
  if (!white) {
		int s = random(0xff);
		for (int i = s; i < s + 0xff; i++) {
			col = Palettes.getColor(i);
			if (col.r + col.g + col.b > 64)
				break;
		}
  }
}

void Twinkle::nudge(int32_t data) {
	size++;
	if (size > 12) size = 1;
}

void Twinkle::update(uint32_t ms) {
	pixelBuffer->fade(fadeRate);
	/*
	if (variableSize)
		size = random(5) + 1;*/

	uint16_t len = pixelBuffer->length / size;
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
		for (int i = 0; i < newPoints; i++) {
			uint16_t index = random(len) * size;
			for (int j = 0; j < size; j++) {
				pixelBuffer->setPixel(index + j, col);
			}
		}
	}

	LightProgram::update(ms);
}
