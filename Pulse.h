#ifndef _PULSE_H_
#define _PULSE_H_

#include "LightString.h"

#define PULSE 0x08

namespace LightString {

class Pulse : public LightProgram {
private:
	bool bounce;
	bool fade;
	bool mirrored;

	float fadeRate;
	uint8_t colorIndex;
	uint8_t changeRate;

	int8_t pulseStep;
	uint16_t pulseIndex;
	uint16_t pulseTotal;
	uint16_t center;

public:
	Pulse(PixelBuffer *pixelBuffer = 0)
		: LightProgram(pixelBuffer) {}

	uint8_t getProgramID() { return PULSE; }

	void setupMode(uint8_t mode);
	void update(uint32_t ms);
};

};

#endif
