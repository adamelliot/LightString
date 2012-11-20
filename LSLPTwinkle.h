#ifndef _LSLPTWINKLE_H_
#define _LSLPTWINKLE_H_

#include "LSLightString.h"

typedef struct twinkle_config_s twinkle_config_t, *ptwinkle_config_t;

struct twinkle_config_s {
	color_t col;
	bool colorCycle;
};

class LSLPTwinkle : public LSLightProgram {
protected:
	twinkle_config_t config;
	
	uint8_t colorIndex;
	uint8_t indexStep;
	
	void setRainbowPixel();

public:
	LSLPTwinkle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);
	
	void setConfig(void *_config);
	void *getConfig();
	
	uint8_t getFrameRate();
	void update();
};

LSLightProgram *factoryTwinkle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
