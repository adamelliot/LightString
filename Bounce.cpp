#include "Bounce.h"

void Bounce::setupMode(uint8_t mode) {
	switch (mode) {
		case 0: // Normal
		mirrored = false;
		fade = false;
		break;
		
		case 1: // Mirrored
		mirrored = true;
		fade = false;
		break;
		
		case 2: // Fade
		mirrored = false;
		fade = true;
		break;
		
		case 3:
		mirrored = true;
		fade = true;
		break;
	}

	fadeRate = 192 + random(12);
	
	colorIndex = random(0xff);
	changeRate = random(7) + 1;
	
	bounceStep = random(2) == 1 ? 1 : -1;
	bounceTotal = mirrored ? pixelBuffer->length >> 1 : pixelBuffer->length;
	bounceIndex = bounceTotal >> 1;
}

void Bounce::update(uint32_t ms) {
	if (fade) pixelBuffer->fade(fadeRate);

	CRGB col = Palettes.getColor(colorIndex);
	if (mirrored) {
		pixelBuffer->setMirroredPixel(bounceIndex, col);
	} else {
		pixelBuffer->setPixel(bounceIndex, col);
	}
	
	bounceIndex += bounceStep;
	if (bounceIndex == 0xffff) {
		bounceStep *= -1;
		bounceIndex += bounceStep;
	} else if (bounceIndex == bounceTotal) {
		bounceStep *= -1;
		bounceIndex += bounceStep;
	}

	colorIndex += changeRate;
	LightProgram::update(ms);
}
