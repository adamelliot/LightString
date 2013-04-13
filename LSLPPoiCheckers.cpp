#include "LSLPPoiCheckers.h"

LSLightProgram *factoryPoiCheckers(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPPoiCheckers(pixelBuffer, colorPalette, colorFunc);
}

LSLPPoiCheckers::LSLPPoiCheckers(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
	: LSLightProgram(pixelBuffer, colorPalette, colorFunc)
{
	pixelBuffer->setSize(2, 15);

	switchIndex = 0;
	colorIndex = random(205) + 50;
	colorStep = 1;
	switcher = 0;
}

uint8_t LSLPPoiCheckers::getFrameRate() {
	return 560;
}

void LSLPPoiCheckers::update() {
	pixelBuffer->drawColumn(0,  0, 5, colorIndex * switcher);
	pixelBuffer->drawColumn(0,  5, 5, colorIndex * (switcher ^ 1));
	pixelBuffer->drawColumn(0, 10, 5, colorIndex * switcher);

	switchIndex++;
	if (switchIndex >= 5) {
		switchIndex = 0;
		switcher ^= 1;
	}

	if (colorIndex < 50 || colorIndex == 0xff)
		colorStep *= -1;
	colorIndex += colorStep;

	pixelBuffer->shiftLeft();
	LSLightProgram::update();
}
