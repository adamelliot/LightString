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

void LSLightProgram::setColorPalette(LSColorPalette *colorPalette) {
	this->colorPalette = colorPalette;
}

LSColorPalette *LSLightProgram::getColorPalette() {
	return this->colorPalette;
}

void LSLightProgram::setZXSound(LSZXSound *zxSound) {
	this->zxSound = zxSound;
}

uint8_t LSLightProgram::getPixelFormats() {
	return RGB_PIXEL_BUFFER; // Should support simple RGB pixels
}

uint8_t LSLightProgram::getModeCount() {
	return 1;
}

uint8_t LSLightProgram::getMode() {
	return this->mode;
}

void LSLightProgram::setupMode(uint8_t mode) {
	this->mode = mode;
}

uint8_t LSLightProgram::getProgramID() {
	return 0;
}

uint16_t LSLightProgram::getNextProgramCode() {
	return 0; // FADE_DOWN
}

bool LSLightProgram::usePreviousPalette() {
	return false;
}

bool LSLightProgram::hideFromProgramList() {
	return false;
}

bool LSLightProgram::isProgramFinished() {
	return false;
}

int32_t LSLightProgram::getProgramLength() {
	return -1;
}

uint8_t LSLightProgram::getFrameRate() {
	return 30;
}

void LSLightProgram::setConfig(void *config) {}
void *LSLightProgram::getConfig() { return NULL; }
void LSLightProgram::update() {}
