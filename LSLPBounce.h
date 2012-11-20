#ifndef _LSLPBOUNCE_H_
#define _LSLPBOUNCE_H_

#include "LSLightString.h"

class LSLPBounce : public LSLightProgram {
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
	LSLPBounce(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);
	void update();

};

LSLightProgram *factoryBounce(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
