#include "LSLEDStrip.h"

LSLEDStrip::LSLEDStrip(CLEDController *controller, uint16_t length) :
	controller(controller), length(length)
{
	controller->init();
	pixels = calloc(length, 3);
}

LSPixelBuffer *LSLEDStrip::getPixelBuffer(uint16_t length, uint16_t offset) {
	if (length == 0) length = this->length;

	LSPixelBuffer *ret = new LSPixelBuffer((void *)((uint8_t *)pixels + offset), length);
	ret->clear();

	return ret;
}

uint16_t LSLEDStrip::getLength(void) {
  return length;
}

void *LSLEDStrip::getPixels(void) {
	return pixels;
}

LSColorPalette *LSLEDStrip::getColorPalette(void) {
	return colorPalette;
}

void LSLEDStrip::setColorPalette(LSColorPalette *colorPalette) {
	this->colorPalette = colorPalette;
}

void LSLEDStrip::update() {
	controller->showRGB((byte *)pixels, length);
}
