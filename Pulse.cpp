#include "Pulse.h"

void Pulse::setupMode(uint8_t mode) {
	fade = random(3) != 2;
	mirrored = 1;//random(2) == 0;
	bounce = random(3) != 1;

	fadeRate = 128 + random(15);
	
	colorIndex = 0; //random(0xff);
	changeRate = 5; //random(8) + 3;
	
	pulseStep = random(2) == 1 ? 1 : -1;
	pulseIndex = 0;
	pulseTotal = mirrored ? pixelBuffer->length >> 1 : pixelBuffer->length;
	center = pulseTotal >> 1;
}

void Pulse::update(uint32_t ms) {
	if (fade) pixelBuffer->fade(fadeRate);

	uint8_t offset = 0;
	if (pulseTotal % 2 == 0) offset = 1;

	CRGB col = Palettes.getColor(colorIndex);

	if (mirrored) {
		pixelBuffer->setMirroredPixel(center + pulseIndex, col);
		pixelBuffer->setMirroredPixel(center - pulseIndex - offset, col);
	} else {
		pixelBuffer->setPixel(center + pulseIndex, col);
		pixelBuffer->setPixel(center - pulseIndex - offset, col);
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
	LightProgram::update(ms);
}
