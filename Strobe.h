#ifndef _STROBE_H_
#define _STROBE_H_

#include "LightString.h"

#define STROBE 0x07

namespace LightString {

class Strobe : public LightProgram {
protected:
	uint8_t colorIndex;
	uint16_t frames, frameCount;

public:
	Strobe(PixelBuffer *pixelBuffer = 0)
		: LightProgram(pixelBuffer) {}

	uint8_t getProgramID() { return STROBE; }

	void setupMode(uint8_t mode);
	void nudge(int32_t data);
	void update(uint32_t ms);
};

};

#endif
