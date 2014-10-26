#include "LSLPPulse.h"

LSLightProgram *factoryPulse(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette) {
	return new LSLPPulse(pixelBuffer, colorPalette);
}

void LSLPPulse::setupMode(uint8_t mode) {
	fade = random(3) != 2;
	mirrored = 1;//random(2) == 0;
	bounce = random(3) != 1;

	fadeRate = 0.5 + ((float)random(201) / 1000);
	
	colorIndex = 0; //random(0xff);
	changeRate = 5; //random(8) + 3;
	
	pulseStep = random(2) == 1 ? 1 : -1;
	pulseIndex = 0;
	pulseTotal = mirrored ? pixelBuffer->getLength() >> 1 : pixelBuffer->getLength();
	center = pulseTotal >> 1;
}

void LSLPPulse::update(uint32_t ms) {
	if (fade) pixelBuffer->fade(fadeRate);

	uint8_t offset = 0;
	if (pulseTotal % 2 == 0) offset = 1;

	if (mirrored) {
		(pixelBuffer->*pixelBuffer->setMirroredIndexedPixel)(center + pulseIndex, colorIndex);
		(pixelBuffer->*pixelBuffer->setMirroredIndexedPixel)(center - pulseIndex - offset, colorIndex);
	} else {
		(pixelBuffer->*pixelBuffer->setIndexedPixel)(center + pulseIndex, colorIndex);
		(pixelBuffer->*pixelBuffer->setIndexedPixel)(center - pulseIndex - offset, colorIndex);
	}
	
	pulseIndex += pulseStep;
	if (pulseIndex == 0xffff) {
		if (bounce) {
			pulseStep *= -1;
			pulseIndex += pulseStep;
		} else
			pulseIndex = center - 1;
	} else if (pulseIndex == center) {
		if (bounce) {
			pulseStep *= -1;
			pulseIndex += pulseStep;
		} else
			pulseIndex = 0;
	}

	colorIndex += changeRate;
	LSLightProgram::update(ms);
}
