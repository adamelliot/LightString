#include "LSLightProgram.h"

LSLightProgram::LSLightProgram(LSPixelBuffer *pixelBuffer, LSColorPalette *colorPalette, pcolor_func colorFunc)
	: pixelBuffer(pixelBuffer), colorPalette(colorPalette), colorFunc(colorFunc)
{}

LSPixelBuffer *LSLightProgram::getPixelBuffer() {
	return pixelBuffer;
}

void LSLightProgram::setColorFunc(pcolor_func colorFunc) {
	this->colorFunc = colorFunc;
}

pcolor_func LSLightProgram::getColorFunc() {
	return colorFunc;
}

void LSLightProgram::setZXSound(LSZXSound *zxSound) {
	this->zxSound = zxSound;
}

uint8_t LSLightProgram::getFrameRate() {
	return 30;
}

void LSLightProgram::setConfig(void *config) {}
void *LSLightProgram::getConfig() { return NULL; }
void LSLightProgram::update() {}
