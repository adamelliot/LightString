#include "LightProgram.h"

using namespace LightString;

LightProgram::LightProgram(PixelBuffer *pixelBuffer)
	: pixelBuffer(pixelBuffer), modeCount(1)
{}

void LightProgram::setpixelBuffer(PixelBuffer *pixelBuffer) {
	this->pixelBuffer = pixelBuffer;
}

PixelBuffer *LightProgram::getPixelBuffer() {
	return pixelBuffer;
}

uint8_t LightProgram::getModeCount() {
	return modeCount;
}

uint8_t LightProgram::getMode() {
	return this->mode;
}

void LightProgram::setupMode(uint8_t mode) {
	this->mode = mode;
}

uint8_t LightProgram::getProgramID() {
	return 0;
}

uint16_t LightProgram::getNextProgramCode() {
	return 0x100; // FADE_DOWN
}

bool LightProgram::usePreviousPalette() {
	return false;
}

bool LightProgram::hideFromProgramList() {
	return false;
}

bool LightProgram::isProgramFinished() {
	return false;
}

int32_t LightProgram::getProgramLength() {
	return -1;
}

void LightProgram::setConfig(void *config) {}
void *LightProgram::getConfig() { return NULL; }

void LightProgram::nudge(int32_t) {}

void LightProgram::update(uint32_t ms) {}
