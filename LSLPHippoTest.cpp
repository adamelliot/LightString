#include "LSLPHippoTest.h"

LSLightProgram *factoryHippoTest(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPHippoTest(pixelBuffer, colorPalette, colorFunc);
}

LSLPHippoTest::LSLPHippoTest(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
	: LSLightProgram(pixelBuffer, colorPalette, colorFunc)
{
	pixelBuffer->setSize(22, 14);

	colorIndex = random(0xff);
	changeRate = random(8) + 3;
}

void LSLPHippoTest::update() {
//	pixelBuffer->fade(0.7f);
//	pixelBuffer->shiftUp();

	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 14; y++) {
			(pixelBuffer->*pixelBuffer->setIndexedPixelAt)(x * 20 + 1, y, colorIndex + (x * 20));
		}
	}

	colorIndex += changeRate;
	LSLightProgram::update();
}
