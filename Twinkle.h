#ifndef _TWINKLE_H_
#define _TWINKLE_H_

#include "LightString.h"

#define TWINKLE 0x03

namespace LightString {

class Twinkle : public LightProgram {
protected:
	CRGB col;
	bool colorCycle;
	bool variableSize;
	bool white;

	uint8_t colorIndex;
	uint8_t indexStep;
	uint8_t size;
	float fadeRate;

	void setRainbowPixel();

public:
	Twinkle(PixelBuffer *pixelBuffer = 0)
		: LightProgram(pixelBuffer) {
			modeCount = 6;
	}

	uint8_t getProgramID() { return TWINKLE; }

	void setupMode(uint8_t mode);
	void nudge(int32_t data);
	void update(uint32_t ms);
};

};

#endif
