#ifndef _LSLPSOLIDCYCLE_H_
#define _LSLPSOLIDCYCLE_H_

#include "LSLightString.h"

#define SOLID_CYCLE 0x04

class LSLPSolidCycle : public LSLightProgram {
protected:
	uint8_t colorIndex;
	uint8_t changeRate;
	bool split, reverse;
	uint8_t offset;

public:
	LSLPSolidCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
		: LSLightProgram(pixelBuffer, colorPalette, colorFunc) {
		modeCount = 3;
	}
	uint8_t getProgramID() { return SOLID_CYCLE; }

	void setupMode(uint8_t mode);
	void update();

};

LSLightProgram *factorySolidCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
