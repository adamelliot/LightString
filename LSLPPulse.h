#ifndef _LSLPPULSE_H_
#define _LSLPPULSE_H_

#include "LSLightString.h"

#define PULSE 0x08

class LSLPPulse : public LSLightProgram {
private:
	bool bounce;
	bool fade;
	bool mirrored;

	float fadeRate;
	uint8_t colorIndex;
	uint8_t changeRate;

	int8_t pulseStep;
	uint16_t pulseIndex;
	uint16_t pulseTotal;
	uint16_t center;

public:
	LSLPPulse(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
		: LSLightProgram(pixelBuffer, colorPalette, colorFunc) {}

	uint8_t getProgramID() { return PULSE; }

	void setupMode(uint8_t mode);
	void update();
};

LSLightProgram *factoryPulse(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
