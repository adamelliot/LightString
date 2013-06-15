#include "LSLPRainbowCycle.h"

LSLightProgram *factoryRainbowCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPRainbowCycle(pixelBuffer, colorPalette, colorFunc);
}

LSLPRainbowCycle::LSLPRainbowCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
	: LSLightProgram(pixelBuffer, colorPalette, colorFunc)
{}

uint8_t LSLPRainbowCycle::getProgramID() {
	return GRADIENT_CYCLE;
}

void LSLPRainbowCycle::setupMode(uint8_t mode) {
	colorIndex = random(0xff);
	sections = (random(2) + 1) * ((pixelBuffer->getLength() / 160) + 1);
	changeRate = random(8) + 3;
	
	Serial.print("Palette: ");
	Serial.println((uint16_t)colorPalette);

	mirrored = !colorPalette->isMirrored();
}

void LSLPRainbowCycle::drawMirrored() {
	uint8_t fact = (0xff / pixelBuffer->getLength()) * sections;
	if (fact < 1) fact = 1;

	for (int i = 0; i < pixelBuffer->getLength() >> 1; i++)
		pixelBuffer->setMirroredPixelWithPaletteIndex(i, i * fact + colorIndex);
}

void LSLPRainbowCycle::drawNormal() {
	uint16_t fact = (0xff / pixelBuffer->getLength()) * sections;
	for (int i = 0; i < pixelBuffer->getLength(); i++)
		pixelBuffer->setPixelWithPaletteIndex(i, i * fact + colorIndex);
}

void LSLPRainbowCycle::update() {
	colorIndex += changeRate;
	mirrored ? drawMirrored() : drawNormal();

	LSLightProgram::update();
}
