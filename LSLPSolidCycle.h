#ifndef _LSLPSOLIDCYCLE_H_
#define _LSLPSOLIDCYCLE_H_

#include "LSLightString.h"

#define SOLID_CYCLE 0x04

class LSLPSolidCycle : public LSLightProgram {
protected:
	uint8_t colorIndex;
	uint8_t changeRate;

public:
	LSLPSolidCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);
	uint8_t getProgramID();
	void setupMode(uint8_t mode);

	void update();

};

LSLightProgram *factorySolidCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
