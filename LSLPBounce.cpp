#include "LSLPBounce.h"

LSLightProgram *factoryBounce(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPBounce(pixelBuffer, colorPalette, colorFunc);
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
	}

	fadeRate = 0.45 + ((float)random(201) / 1000);
	
	colorIndex = 0; //random(0xff);
	changeRate = 5; //random(8) + 3;
	
	bounceStep = random(2) == 1 ? 1 : -1;
	bounceTotal = mirrored ? pixelBuffer->getLength() >> 1 : pixelBuffer->getLength();
	bounceIndex = bounceTotal >> 1;
}

uint8_t LSLPBounce::modeCount() {
	return 3;
}

void LSLPBounce::update() {
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
