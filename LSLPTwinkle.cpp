#include "LSLPTwinkle.h"

LSLightProgram *factoryTwinkle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPTwinkle(pixelBuffer, colorPalette, colorFunc);
}

LSLPTwinkle::LSLPTwinkle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
	: LSLightProgram(pixelBuffer, colorPalette, colorFunc)
{
	colorIndex = 0;
	indexStep = random(3) + 1;

	config.colorCycle = random(5) < 2;
	config.col = (color_t){0xff, 0xff, 0xff};
	if (random(5) < 3)
		config.col = colorPalette->getColor(random(0x100));
}

void LSLPTwinkle::setConfig(void *_config) {
	ptwinkle_config_t config = (ptwinkle_config_t)_config;
	this->config.col = config->col;
	this->config.colorCycle = config->colorCycle;
}

void *LSLPTwinkle::getConfig() {
	return &config;
}

uint8_t LSLPTwinkle::getFrameRate() {
	return 20;
}

void LSLPTwinkle::update() {
	pixelBuffer->fade(0.8f);

	uint8_t newPoints = random(pixelBuffer->getLength() >> 3) + 1;

	if (config.colorCycle) {
		for (int i = 0; i < newPoints; i++)
			pixelBuffer->setPixel(random(pixelBuffer->getLength()), colorPalette->getColor(colorIndex += indexStep));
	} else {
		for (int i = 0; i < newPoints; i++)
			pixelBuffer->setPixel(random(pixelBuffer->getLength()), colorFunc(config.col.channels[0], config.col.channels[1], config.col.channels[2]));
	}

	LSLightProgram::update();
}
