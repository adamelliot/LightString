#ifndef _LSLPTWINKLE_H_
#define _LSLPTWINKLE_H_

#include "LSLightString.h"

#define TWINKLE 0x03

class LSLPTwinkle : public LSLightProgram {
protected:
	color_t col;
	bool colorCycle;
	bool variableSize;
	bool white;

	uint8_t colorIndex;
	uint8_t indexStep;
	uint8_t size;
	uint8_t fastFade;

	void setRainbowPixel();

public:
	LSLPTwinkle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
		: LSLightProgram(pixelBuffer, colorPalette, colorFunc) {
			modeCount = 6;
	}

	uint8_t getProgramID() { return TWINKLE; }

	void setupMode(uint8_t mode);
	void nudge(int32_t data);
	void update();
};

LSLightProgram *factoryTwinkle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
