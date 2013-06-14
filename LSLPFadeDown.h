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
	LSLPFadeDown(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

	uint8_t getProgramID();
	void setupMode(uint8_t mode);
	
	bool usePreviousPalette();
	bool hideFromProgramList();

	uint8_t getFrameRate();
	bool isProgramFinished();
		
	void update();
};

LSLightProgram *factoryTwinkle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
