#include "LSLEDStrip.h"

LSLEDStrip::LSLEDStrip(CLEDController *controller, uint16_t length) :
	controller(controller), length(length)
{
	controller->init();
	pixels = (CRGB *)calloc(length, 3);
}

LSPixelBuffer *LSLEDStrip::getPixelBuffer(uint16_t length, uint16_t offset) {
	if (length == 0) length = this->length;

	LSPixelBuffer *ret = new LSPixelBuffer((CRGB *)((uint8_t *)pixels + offset), length);
	ret->clear();

	return ret;
}

uint16_t LSLEDStrip::getLength(void) {
  return length;
}

void *LSLEDStrip::getPixels(void) {
	return pixels;
}

void LSLEDStrip::update() {/*
	for (int j = 0; j < length * 3; j += 6) {
		((uint8_t *)pixels)[j] = 0;
		((uint8_t *)pixels)[j + 1] = 0;
		((uint8_t *)pixels)[j + 2] = 0;
	}*/
	
	controller->show((CRGB *)pixels, length, 255);
}
