#ifndef _BOUNCE_H_
#define _BOUNCE_H_

#include "LightString.h"

#define BOUNCE 0x05

namespace LightString {

class Bounce : public LightProgram {
protected:
	bool fade;
	bool mirrored;

	uint8_t fadeRate;
	uint8_t colorIndex;
	uint8_t changeRate;

	int8_t bounceStep;
	uint16_t bounceIndex;
	uint16_t bounceTotal;

public:
	Bounce(PixelBuffer *pixelBuffer = 0)
		: LightProgram(pixelBuffer) {
		modeCount = 4; 
	}
	uint8_t getProgramID() { return BOUNCE; }

	void setupMode(uint8_t mode);
	void update(uint32_t ms);
};

};

#endif
