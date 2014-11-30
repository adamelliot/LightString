#ifndef _LSLPFADEDOWN_H_
#define _LSLPFADEDOWN_H_

#include "LSLightString.h"

#define FADE_DOWN 0x01

class LSLPFadeDown : public LSLightProgram {
protected:
	bool faded;
	uint8_t step, steps;
	uint16_t totalTime;

public:
	LSLPFadeDown(LSPixelBuffer *pixelBuffer)
		: LSLightProgram(pixelBuffer) {}

	uint8_t getProgramID() { return FADE_DOWN; }

	void setupMode(uint8_t mode);
	
	uint16_t getNextProgramCode();
	bool usePreviousPalette();
	bool hideFromProgramList();

	uint8_t getFrameRate();
	bool isProgramFinished();
		
	void update(uint32_t ms);
};

LSLightProgram *factoryFadeDown(LSPixelBuffer *pixelBuffer);

#endif
