#include "LSLPFadeDown.h"

LSLightProgram *factoryFadeDown(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPFadeDown(pixelBuffer, colorPalette, colorFunc);
}

LSLPFadeDown::LSLPFadeDown(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
	: LSLightProgram(pixelBuffer, colorPalette, colorFunc)
{}

uint8_t LSLPFadeDown::getProgramID() {
	return FADE_DOWN_ID;
}

void LSLPFadeDown::setupMode(uint8_t mode) {
	step = 0;
	steps = 30;
	totalTime = 1000;
	faded = false;
}

bool LSLPFadeDown::canBeRandomlySelected() {
	return false;
}

uint8_t LSLPFadeDown::getFrameRate() {
	return 30;
}

bool LSLPFadeDown::isProgramFinished() {
	return faded;
}

void LSLPFadeDown::update() {
	float ratio = (float)(steps - (step + 1)) / (float)(steps - step);
	pixelBuffer->fade(ratio);
	step++;

	if (step >= steps) faded = true;

	LSLightProgram::update();
}
