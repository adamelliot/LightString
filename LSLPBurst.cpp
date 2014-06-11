#include "LSLPBounce.h"

LSLightProgram *factoryBurst(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPBurst(pixelBuffer, colorPalette, colorFunc);
}

void LSLPBurst::setupMode(uint8_t mode) {
	fade = true;

	switch (mode) {
		case 0: // Normal
		mirrored = false;
		break;
		
		case 1: // Mirrored
		mirrored = true;
		break;
	}

	fadeRate = 0.75 + ((float)random(201) / 1000);
	
	colorIndex = 0; //random(0xff);
	changeRate = 5; //random(8) + 3;

	bounceStep = random(2) == 1 ? 1 : -1;
	bounceTotal = mirrored ? pixelBuffer->getLength() >> 1 : pixelBuffer->getLength();
	bounceIndex = bounceTotal >> 1;
}

void LSLPBurst::update() {
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
	LSLightProgram::update();
}
