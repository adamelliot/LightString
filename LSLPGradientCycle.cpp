#include "LSLPGradientCycle.h"

LSLightProgram *factoryGradientCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPGradientCycle(pixelBuffer, colorPalette, colorFunc);
}

void LSLPGradientCycle::setupMode(uint8_t mode) {
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
	sections = (random(2) + 1) * ((pixelBuffer->getLength() / 160) + 1);
	changeRate = random(7) + 1;
}

void LSLPGradientCycle::drawMirrored() {
	uint8_t fact = (0xff / pixelBuffer->getLength()) * sections;
	if (fact < 1) fact = 1;

	for (int i = 0; i < pixelBuffer->getLength() >> 1; i++)
		pixelBuffer->setMirroredPixelWithPaletteIndex(i, i * fact + colorIndex);
}

void LSLPGradientCycle::drawNormal() {
	uint16_t fact = (0xff / pixelBuffer->getLength()) * sections;
	for (int i = 0; i < pixelBuffer->getLength(); i++)
		pixelBuffer->setPixelWithPaletteIndex(i, i * fact + colorIndex);
}

void LSLPGradientCycle::update(uint32_t ms) {
	colorIndex += changeRate;
	mirrored ? drawMirrored() : drawNormal();

	LSLightProgram::update(ms);
}
