#include "LSLPRainbowCycle.h"

LSLightProgram *factoryRainbowCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPRainbowCycle(pixelBuffer, colorPalette, colorFunc);
}

LSLPRainbowCycle::LSLPRainbowCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
	: LSLightProgram(pixelBuffer, colorPalette, colorFunc)
{
	colorIndex = random(0xff);
	sections = (random(2) + 1) * ((pixelBuffer->getLength() / 160) + 1);
	changeRate = random(8) + 3;

	mirrored = !colorPalette->isMirrored();
}

void LSLPRainbowCycle::setConfig(void *_config) {
	prainbow_cycle_config_t config = (prainbow_cycle_config_t)_config;
	if (config->mirrored) mirrored = true;
}

void LSLPRainbowCycle::drawMirrored() {
	uint8_t fact = (0xff / pixelBuffer->getLength()) * sections;
	for (int i = 0; i < pixelBuffer->getLength() >> 1; i++)
//		pixelBuffer->setMirroredPixel(i, colorPalette->getColor(i * fact + colorIndex));
		pixelBuffer->setMirroredPixel(i, i * fact + colorIndex);
}

void LSLPRainbowCycle::drawNormal() {
	uint8_t fact = (0xff / pixelBuffer->getLength()) * sections;
	for (int i = 0; i < pixelBuffer->getLength(); i++)
//		pixelBuffer->setPixel(i, colorPalette->getColor(i * fact + colorIndex));
		pixelBuffer->setPixel(i, i * fact + colorIndex);
}

void LSLPRainbowCycle::update() {
	colorIndex += changeRate;
	mirrored ? drawMirrored() : drawNormal();

	LSLightProgram::update();
}
