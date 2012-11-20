#ifndef _LSLPSOLIDCYCLE_H_
#define _LSLPSOLIDCYCLE_H_

#include "LSLightString.h"

class LSLPSolidCycle : public LSLightProgram {
protected:
	uint8_t colorIndex;
	uint8_t changeRate;

public:
	LSLPSolidCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);
	void update();

};

LSLightProgram *factorySolidCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
