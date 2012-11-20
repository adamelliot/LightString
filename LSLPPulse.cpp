#include "LSLPPulse.h"

LSLightProgram *factoryPulse(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPPulse(pixelBuffer, colorPalette, colorFunc);
}

LSLPPulse::LSLPPulse(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
	: LSLightProgram(pixelBuffer, colorPalette, colorFunc)
{
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

void LSLPPulse::update() {
	if (fade) pixelBuffer->fade(fadeRate);

	uint8_t offset = 0;
	if (pulseTotal % 2 == 0) offset = 1;

	if (mirrored) {
		pixelBuffer->setMirroredPixel(center + pulseIndex, colorPalette->getColor(colorIndex));
		pixelBuffer->setMirroredPixel(center - pulseIndex - offset, colorPalette->getColor(colorIndex));
	} else {
		pixelBuffer->setPixel(center + pulseIndex, colorPalette->getColor(colorIndex));
		pixelBuffer->setPixel(center - pulseIndex - offset, colorPalette->getColor(colorIndex));
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
	LSLightProgram::update();
}
