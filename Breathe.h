#ifndef _BREATHE_H_
#define _BREATHE_H_

#include "LightString.h"

#define BREATHE 0x06

namespace LightString {

class Breathe : public LightProgram {
protected:
	float factor;
	float step, direction;
	float min; // Max is always 1.0

public:
	Breathe(PixelBuffer *pixelBuffer = 0)
		: LightProgram(pixelBuffer) {}

	uint8_t getProgramID() { return BREATHE; }

	void setupMode(uint8_t mode);
	void update(uint32_t ms);
};

};

#endif
