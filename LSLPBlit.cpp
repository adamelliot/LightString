#include "LSLPBlit.h"

LSLightProgram *factoryBlit(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPBlit(pixelBuffer, colorPalette, colorFunc);
}

LSLPBlit::LSLPBlit(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
	: LSLightProgram(pixelBuffer, colorPalette, colorFunc)
{
	pixelBuffer->setSize(22, 14);

	colorIndex = 0;
	indexStep = random(3) + 6;
}

void LSLPBlit::setConfig(void *_config) {
	pblit_config_t config = (pblit_config_t)_config;
}

void *LSLPBlit::getConfig() {
	return &config;
}

uint8_t LSLPBlit::getFrameRate() {
	return 10;
}

void LSLPBlit::update() {
	static int x = 0;
	pixelBuffer->drawText(0, 0, x++, 0, 1, 14, "AKLJHIHG");
	x %= 88;

	pixelBuffer->shiftLeft();
	LSLightProgram::update();
}
