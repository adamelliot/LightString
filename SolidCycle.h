#ifndef _SOLIDCYCLE_H_
#define _SOLIDCYCLE_H_

#include "LightString.h"

#define SOLID_CYCLE 0x04

namespace LightString {

class SolidCycle : public LightProgram {
protected:
	uint8_t colorIndex;
	uint8_t changeRate;
	bool split, reverse;
	uint8_t offset;

public:
	SolidCycle(PixelBuffer *pixelBuffer = 0)
		: LightProgram(pixelBuffer) {
		modeCount = 3;
	}
	uint8_t getProgramID() { return SOLID_CYCLE; }

	void setupMode(uint8_t mode);
	void update(uint32_t ms);

};

};

#endif
