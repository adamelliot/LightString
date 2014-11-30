#include "LSLPFadeDown.h"

LSLightProgram *factoryFadeDown(LSPixelBuffer *pixelBuffer) {
	return new LSLPFadeDown(pixelBuffer);
}

void LSLPFadeDown::setupMode(uint8_t mode) {
	step = 0;
	steps = 30;
	totalTime = 1000;
	faded = false;
}

uint16_t LSLPFadeDown::getNextProgramCode() {
	return 0;
}

bool LSLPFadeDown::usePreviousPalette() {
	return true;
}

bool LSLPFadeDown::hideFromProgramList() {
	return true;
}

uint8_t LSLPFadeDown::getFrameRate() {
	return 30;
}

bool LSLPFadeDown::isProgramFinished() {
	return faded;
}

void LSLPFadeDown::update(uint32_t ms) {
	float ratio = (float)(steps - (step + 1)) / (float)(steps - step);
	pixelBuffer->fade(ratio);
	step++;

	if (step >= steps) faded = true;

	LSLightProgram::update(ms);
}
