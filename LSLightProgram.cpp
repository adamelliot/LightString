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

virtual uint8_t LSLightProgram::getPixelFormats() {
	return RGB_PIXEL_BUFFER; // Should support simple RGB pixels
}

virtual uint8_t LSLightProgram::getModeCount() {
	return 1;
}

virtual void LSLightProgram::setupMode(uint8_t mode) {
	this->mode = mode;
}

virtual int8_t LSLightProgram::getProgramID() {
	return 0xff;
}

virtual uint16_t LSLightProgram::getNextProgramCode() {
	return RANDOM_PROGRAM;
}

virtual bool hideFromProgramQueue() {
	return false;
}

virtual bool isProgramFinished() {
	return false;
}

virtual int32_t getProgramLength() {
	return -1;
}

uint8_t LSLightProgram::getFrameRate() {
	return 30;
}

void LSLightProgram::setConfig(void *config) {}
void *LSLightProgram::getConfig() { return NULL; }
void LSLightProgram::update() {}
