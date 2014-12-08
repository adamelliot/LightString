#ifndef _LSLPTWINKLE_H_
#define _LSLPTWINKLE_H_

#include "LSLightString.h"

#define TWINKLE 0x03

class LSLPTwinkle : public LSLightProgram {
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
	LSLPTwinkle(LSPixelBuffer *pixelBuffer = 0)
		: LSLightProgram(pixelBuffer) {
			modeCount = 6;
	}

	uint8_t getProgramID() { return TWINKLE; }

	void setupMode(uint8_t mode);
	void nudge(int32_t data);
	void update(uint32_t ms);
};

LSLightProgram *factoryTwinkle(LSPixelBuffer *pixelBuffer);

#endif
