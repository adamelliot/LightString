#ifndef _LSLPGRADIENTCYCLE_H_
#define _LSLPGRADIENTCYCLE_H_

#include "LSLightString.h"

#define GRADIENT_CYCLE 0x02

class LSLPGradientCycle : public LSLightProgram {
protected:
	bool mirrored;
	uint8_t sections;
	uint8_t colorIndex;
	uint8_t changeRate;
	
	void drawMirrored();
	void drawNormal();

public:
	LSLPGradientCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
		: LSLightProgram(pixelBuffer, colorPalette, colorFunc) {
		modeCount = 2; 
	}

	uint8_t getProgramID() { return GRADIENT_CYCLE; }
	void setupMode(uint8_t mode);
	void update();
};

LSLightProgram *factoryGradientCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif