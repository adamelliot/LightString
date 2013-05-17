#ifndef _LSLPFADEDOWN_H_
#define _LSLPFADEDOWN_H_

#include "LSLightString.h"

#define FADE_DOWN_ID 0x00

class LSLPFadeDown : public LSLightProgram {
protected:
	bool faded;
	uint8_t step, steps;
	uint16_t totalTime;

public:
	LSLPFadeDown(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);
	
	uint8_t getFrameRate();
	void update();
};

LSLightProgram *factoryTwinkle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
