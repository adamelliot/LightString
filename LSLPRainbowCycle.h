#ifndef _LSLPRAINBOWCYCLE_H_
#define _LSLPRAINBOWCYCLE_H_

#include "LSLightString.h"

typedef struct rainbow_cycle_config_s rainbow_cycle_config_t, *prainbow_cycle_config_t;

struct rainbow_cycle_config_s {
	bool mirrored;
};

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
	void setConfig(void *_config);
	void update();

};

LSLightProgram *factoryRainbowCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
