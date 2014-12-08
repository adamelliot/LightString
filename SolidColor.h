#ifndef _SOLIDCOLOR_H_
#define _SOLIDCOLOR_H_

#include "LightString.h"

#define SOLID_COLOR 0x09
#define MAX_COLORS 10

namespace LightString {

class SolidColor : public LightProgram {
protected:
	uint8_t colorIndex;
	CRGB colors[MAX_COLORS];

public:
	SolidColor(PixelBuffer *pixelBuffer = 0)
		: LightProgram(pixelBuffer) {}

	uint8_t getProgramID() { return SOLID_COLOR; }

	void setupMode(uint8_t mode);
	void nudge(int32_t data);
	void update(uint32_t ms);

};

};

#endif
