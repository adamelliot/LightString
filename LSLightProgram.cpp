#include "LSLightProgram.h"

LSLightProgram::LSLightProgram(LSPixelBuffer *pixelBuffer)
	: pixelBuffer(pixelBuffer), modeCount(1)
{}

void LSLightProgram::setPixelBuffer(LSPixelBuffer *pixelBuffer) {
	this->pixelBuffer = pixelBuffer;
}

LSPixelBuffer *LSLightProgram::getPixelBuffer() {
	return pixelBuffer;
}

uint8_t LSLightProgram::getModeCount() {
	return modeCount;
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
	return 0x100; // FADE_DOWN
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

void LSLightProgram::setConfig(void *config) {}
void *LSLightProgram::getConfig() { return NULL; }

void LSLightProgram::nudge(int32_t) {}

void LSLightProgram::update(uint32_t ms) {}
