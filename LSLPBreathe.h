#ifndef _LSLPBREATHE_H_
#define _LSLPBREATHE_H_

#include "LSLightString.h"

#define BREATHE 0x06

class LSLPBreathe : public LSLightProgram {
protected:
	float factor;
	float step, direction;
	float min; // Max is always 1.0

public:
	LSLPBreathe(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
		: LSLightProgram(pixelBuffer, colorPalette, colorFunc) {}

	uint8_t getProgramID() { return BREATHE; }

	void setupMode(uint8_t mode);
	void update();
};

LSLightProgram *factoryBreathe(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
