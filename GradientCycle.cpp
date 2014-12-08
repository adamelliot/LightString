#include "GradientCycle.h"

void GradientCycle::setupMode(uint8_t mode) {
	switch (mode) {
		case 0:
		mirrored = true;
		break;

		case 1:
		mirrored = false;
		//mirrored = !colorPalette->isMirrored();
		break;
	}
	
	colorIndex = random(0xff);
	sections = 1;//(random(2) + 1) * ((pixelBuffer->getLength() / 160) + 1);
	changeRate = random(7) + 1;
}

void GradientCycle::drawMirrored() {
	uint8_t fact = (0xff / pixelBuffer->getLength()) * sections;
	if (fact < 1) fact = 1;

	for (int i = 0; i < pixelBuffer->getLength() >> 1; i++) {
		CRGB col = Palettes.getColor(i * fact + colorIndex);
		pixelBuffer->setMirroredPixel(i, col);
	}
}

void GradientCycle::drawNormal() {
	uint16_t fact = (0xff / pixelBuffer->getLength()) * sections;
	for (int i = 0; i < pixelBuffer->getLength(); i++) {
		CRGB col = Palettes.getColor(i * fact + colorIndex);
		pixelBuffer->setPixel(i, col);
	}
}

void GradientCycle::update(uint32_t ms) {
	colorIndex += changeRate;
	mirrored ? drawMirrored() : drawNormal();

	LightProgram::update(ms);
}
