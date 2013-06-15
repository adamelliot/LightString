#ifndef _LSLPTWINKLE_H_
#define _LSLPTWINKLE_H_

#include "LSLightString.h"

#define TWINKLE 0x03

class LSLPTwinkle : public LSLightProgram {
protected:
	color_t col;
	bool colorCycle;

	uint8_t colorIndex;
	uint8_t indexStep;
	
	void setRainbowPixel();

public:
	LSLPTwinkle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

	uint8_t getProgramID();
	void setupMode(uint8_t mode);

	uint8_t getFrameRate();
	void update();
};

LSLightProgram *factoryTwinkle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
