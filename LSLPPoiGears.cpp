#include "LSLPPoiGears.h"

LSLightProgram *factoryPoiGears(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPPoiGears(pixelBuffer, colorPalette, colorFunc);
}

LSLPPoiGears::LSLPPoiGears(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
	: LSLightProgram(pixelBuffer, colorPalette, colorFunc)
{
	pixelBuffer->setSize(2, 15);

	switchIndex = 0;
	colorIndex = random(205) + 50;
	colorStep = 1;
	switcher = 0;
}

uint8_t LSLPPoiGears::getFrameRate() {
	return 560;
}

void LSLPPoiGears::update() {
	uint8_t grad;
	uint8_t step = colorIndex / 14;

	if (switcher) {
		(pixelBuffer->*pixelBuffer->setIndexedPixelAt)(0, 0, 0);
		if (switchIndex == 0 || switchIndex == 6) {
			pixelBuffer->drawColumn(0, 0, 7, (uint8_t)0xff);
			//pixelBuffer->drawColumn(0, 7, 3, colorIndex);
			grad = 7;
		} else {
			(pixelBuffer->*pixelBuffer->setIndexedPixelAt)(0, 0, 0xff);
			//pixelBuffer->drawColumn(0, 2, 12, colorIndex);
			grad = 0;
		}
	} else {
		pixelBuffer->drawColumn(0, 0, 6, (uint8_t)0);
		(pixelBuffer->*pixelBuffer->setIndexedPixelAt)(0, 6, 0xff);
		//pixelBuffer->drawColumn(0, 7, 7, colorIndex);
		grad = 7;
	}
	
	for (; grad < 14; grad++) {
		(pixelBuffer->*pixelBuffer->setIndexedPixelAt)(0, 1 + grad, colorIndex - (grad * step));
	}
	

	switchIndex++;
	if (switchIndex >= 7) {
		switchIndex = 0;
		switcher ^= 1;
	}

	if (colorIndex < 50 || colorIndex == 0xff)
		colorStep *= -1;
	colorIndex += colorStep;

	pixelBuffer->shiftLeft();
	LSLightProgram::update();
}
