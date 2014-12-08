#ifndef _FADEDOWN_H_
#define _FADEDOWN_H_

#include "LightString.h"

#define FADE_DOWN 0x01

namespace LightString {

class FadeDown : public LightProgram {
protected:
	bool faded;
	uint8_t step, steps;
	uint16_t totalTime;

public:
	FadeDown(PixelBuffer *pixelBuffer = 0)
		: LightProgram(pixelBuffer) {}

	uint8_t getProgramID() { return FADE_DOWN; }

	void setupMode(uint8_t mode);
	
	uint16_t getNextProgramCode();
	bool usePreviousPalette();
	bool hideFromProgramList();

	uint8_t getFrameRate();
	bool isProgramFinished();
		
	void update(uint32_t ms);
};

};

#endif
