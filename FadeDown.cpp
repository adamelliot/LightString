#include "FadeDown.h"

void FadeDown::setupMode(uint8_t mode) {
	step = 0;
	steps = 30;
	totalTime = 1000;
	faded = false;
}

uint16_t FadeDown::getNextProgramCode() {
	return 0;
}

bool FadeDown::usePreviousPalette() {
	return true;
}

bool FadeDown::hideFromProgramList() {
	return true;
}

uint8_t FadeDown::getFrameRate() {
	return 30;
}

bool FadeDown::isProgramFinished() {
	return faded;
}

void FadeDown::update(uint32_t ms) {
	float ratio = (float)(steps - (step + 1)) / (float)(steps - step);
	pixelBuffer->fade(ratio);
	step++;

	if (step >= steps) faded = true;

	LightProgram::update(ms);
}
