#ifndef _LSLPSOUNDSTRIP_H_
#define _LSLPSOUNDSTRIP_H_

#include "LSLightString.h"
/*
typedef struct rainbow_cycle_config_s rainbow_cycle_config_t, *prainbow_cycle_config_t;

struct rainbow_cycle_config_s {
	bool mirrored;
};
*/
class LSLPSoundStrip : public LSLightProgram {
protected:
	bool mirrored;
	uint8_t sections;
	uint8_t colorIndex;
	uint8_t changeRate;

public:
	LSLPSoundStrip(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);
	void setConfig(void *_config);
	void update();

};

LSLightProgram *factorySoundStrip(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
