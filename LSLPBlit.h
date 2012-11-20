#ifndef _LSLPBLIT_H_
#define _LSLPBLIT_H_

#include "LSLightString.h"

typedef struct blit_config_s blit_config_t, *pblit_config_t;
typedef uint8_t *(*pblit_func)(uint16_t&, uint16_t&);

struct blit_config_s {
	bool colorCycle;
};

const uint8_t *blit_a(uint16_t &width, uint16_t &height);

class LSLPBlit : public LSLightProgram {
protected:
	blit_config_t config;

	uint8_t colorIndex;
	uint8_t indexStep;
	
public:
	LSLPBlit(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);
	
	void setConfig(void *_config);
	void *getConfig();
	
	uint8_t getFrameRate();
	void update();
};

LSLightProgram *factoryBlit(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
