#ifndef _LSLPRAINBOWCYCLE_H_
#define _LSLPRAINBOWCYCLE_H_

#include "LSLightString.h"

#define GRADIENT_CYCLE 0x02

class LSLPRainbowCycle : public LSLightProgram {
protected:
	bool mirrored;
	uint8_t sections;
	uint8_t colorIndex;
	uint8_t changeRate;
	
	void drawMirrored();
	void drawNormal();

public:
	LSLPRainbowCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

	uint8_t getProgramID();
	void setupMode(uint8_t mode);

	void update();

};

LSLightProgram *factoryRainbowCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
