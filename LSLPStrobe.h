#ifndef _LSLPSTROBE_H_
#define _LSLPSTROBE_H_

#include "LSLightString.h"

#define STROBE 0x07

class LSLPStrobe : public LSLightProgram {
protected:
	uint8_t colorIndex;
	uint16_t frames, frameCount;

public:
	LSLPStrobe(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
		: LSLightProgram(pixelBuffer, colorPalette, colorFunc) {}

	uint8_t getProgramID() { return STROBE; }

	void setupMode(uint8_t mode);
	void nudge(int32_t data);
	void update(uint32_t ms);
};

LSLightProgram *factoryStrobe(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
