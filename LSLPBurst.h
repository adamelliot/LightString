#ifndef _LSLPBOUNCE_H_
#define _LSLPBOUNCE_H_

#include "LSLightString.h"

#define BURST 0x0a

class LSLPBurst : public LSLightProgram {
protected:
	bool fade;
	bool mirrored;

	float fadeRate;
	uint8_t colorIndex;
	uint8_t changeRate;

	int8_t bounceStep;
	uint16_t bounceIndex;
	uint16_t bounceTotal;

public:
	LSLPBurst(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
		: LSLightProgram(pixelBuffer, colorPalette, colorFunc) {
		modeCount = 2;
	}
	uint8_t getProgramID() { return BURST; }

	void setupMode(uint8_t mode);
	void update();
};

LSLightProgram *factoryBurst(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
