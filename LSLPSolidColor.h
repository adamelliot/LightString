#ifndef _LSLPSOLIDCOLOR_H_
#define _LSLPSOLIDCOLOR_H_

#include "LSLightString.h"

#define SOLID_COLOR 0x09
#define MAX_COLORS 10

class LSLPSolidColor : public LSLightProgram {
protected:
	uint8_t colorIndex;
	CRGB colors[MAX_COLORS];

public:
	LSLPSolidColor(LSPixelBuffer *pixelBuffer)
		: LSLightProgram(pixelBuffer) {}

	uint8_t getProgramID() { return SOLID_COLOR; }

	void setupMode(uint8_t mode);
	void nudge(int32_t data);
	void update(uint32_t ms);

};

LSLightProgram *factorySolidColor(LSPixelBuffer *pixelBuffer);

#endif
