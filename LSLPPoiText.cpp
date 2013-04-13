#include "LSLPPoiText.h"

LSLightProgram *factoryPoiText(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc) {
	return new LSLPPoiText(pixelBuffer, colorPalette, colorFunc);
}

LSLPPoiText::LSLPPoiText(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
	: LSLightProgram(pixelBuffer, colorPalette, colorFunc)
{
	pixelBuffer->setSize(2, 15);

	colorIndex = 0;
	indexStep = random(3) + 6;
	
	switch(random(2)) {
		case 0:
		message = "ASTRO + WILDCARD";
		break;

		case 1:
		message = "GOOSEBALLING";
		break;

		case 2:
		message = "?????????????????????????";
		break;
	}
	
	message = "8888888888888888";
}

uint8_t LSLPPoiText::getFrameRate() {
	// 15km/h = 15000m/h = 4.1666m/s
	// @2m C -> ~2 revolutions per second
	// 12 frames per letter
	// 24 frames per circle
	// = 12 * 24 * 2 = 576
	return 576;// 576;
}

void LSLPPoiText::update() {
	static int x = 0;
	pixelBuffer->drawText(0, 0, x++, 0, 1, 14, message);//"ADAM + NIKOLE   ASTRO + WILDCARD");
	x %= 120;

	pixelBuffer->shiftLeft();
	LSLightProgram::update();
}
