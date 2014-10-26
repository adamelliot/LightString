#include "LSLPBounce.h"

LSLightProgram *factoryBounce(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette) {
	return new LSLPBounce(pixelBuffer, colorPalette);
}

void LSLPBounce::setupMode(uint8_t mode) {
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

	fadeRate = 0.75 + ((float)random(201) / 1000);
	
	colorIndex = random(0xff);
	changeRate = random(7) + 1;
	
	bounceStep = random(2) == 1 ? 1 : -1;
	bounceTotal = mirrored ? pixelBuffer->getLength() >> 1 : pixelBuffer->getLength();
	bounceIndex = bounceTotal >> 1;
}

void LSLPBounce::update(uint32_t ms) {
	if (fade) pixelBuffer->fade(fadeRate);

	if (mirrored) {
		(pixelBuffer->*pixelBuffer->setMirroredIndexedPixel)(bounceIndex, colorIndex);
	} else {
		(pixelBuffer->*pixelBuffer->setIndexedPixel)(bounceIndex, colorIndex);
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
	LSLightProgram::update(ms);
}
