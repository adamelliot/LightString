#ifndef _GRADIENTCYCLE_H_
#define _GRADIENTCYCLE_H_

#include "LightString.h"

#define GRADIENT_CYCLE 0x02

namespace LightString {

class GradientCycle : public LightProgram {
protected:
	bool mirrored;
	uint8_t sections;
	uint8_t colorIndex;
	uint8_t changeRate;
	
	void drawMirrored();
	void drawNormal();

public:
	GradientCycle(PixelBuffer *pixelBuffer = 0)
		: LightProgram(pixelBuffer) {
		modeCount = 2; 
	}

	uint8_t getProgramID() { return GRADIENT_CYCLE; }
	void setupMode(uint8_t mode);
	void update(uint32_t ms);
};

};

#endif
