#include "LSLPTwinkle.h"

LSLightProgram *factoryTwinkle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPTwinkle(pixelBuffer, colorPalette, colorFunc);
}

void LSLPTwinkle::setupMode(uint8_t mode) {
	colorIndex = 0;
	indexStep = random(3) + 1;

	colorCycle = random(5) < 2;
	col = (color_t){0xff, 0xff, 0xff};
	if (random(5) < 3)
		col = colorPalette->getColor(random(0x100));
}

uint8_t LSLPTwinkle::getFrameRate() {
	return 20;
}

void LSLPTwinkle::update() {
	pixelBuffer->fade((uint8_t)1);

	uint8_t newPoints = random(pixelBuffer->getLength() >> 2) + 1;

	if (colorCycle) {
		for (int i = 0; i < newPoints; i++)
			pixelBuffer->setPixelWithPaletteIndex(random(pixelBuffer->getLength()), colorIndex += indexStep);
	} else {
		for (int i = 0; i < newPoints; i++)
			pixelBuffer->setPixelWithPaletteIndex(random(pixelBuffer->getLength()), 128/*colorFunc(col.channels[0], col.channels[1], col.channels[2])*/);
	}

	LSLightProgram::update();
}
