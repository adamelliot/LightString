#include "LSLPFire.h"

LSLightProgram *factoryFire(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPFire(pixelBuffer, colorPalette, colorFunc);
}

LSLPFire::LSLPFire(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
	: LSLightProgram(pixelBuffer, colorPalette, colorFunc)
{
	pixelBuffer->setSize(22, 14);

	colorIndex = random(0xff);
	changeRate = random(8) + 3;
}

void LSLPFire::update() {
	pixelBuffer->fade(0.7f);
	pixelBuffer->shiftUp();

	for (int i = 0; i < 5; i++) {
		(pixelBuffer->*pixelBuffer->setIndexedPixelAt)(random(pixelBuffer->getWidth()), pixelBuffer->getHeight() - 1, random(0x100));
	}

	colorIndex += changeRate;
	LSLightProgram::update();
}
