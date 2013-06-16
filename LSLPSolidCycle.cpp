#include "LSLPSolidCycle.h"

LSLightProgram *factorySolidCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPSolidCycle(pixelBuffer, colorPalette, colorFunc);
}

LSLPSolidCycle::LSLPSolidCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
	: LSLightProgram(pixelBuffer, colorPalette, colorFunc)
{}

uint8_t LSLPSolidCycle::getProgramID() {
	return SOLID_CYCLE;
}

void LSLPSolidCycle::setupMode(uint8_t mode) {
	colorIndex = random(0xff);
	changeRate = random(4) + 3;
}

void LSLPSolidCycle::update() {
	colorIndex += changeRate;
	
	for (int i = 0; i < pixelBuffer->getLength(); i++)
		(pixelBuffer->*pixelBuffer->setIndexedPixel)(i, colorIndex);

	LSLightProgram::update();
}
