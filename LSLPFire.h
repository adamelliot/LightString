#ifndef _LSLPFIRE_H_
#define _LSLPFIRE_H_

#include "LSLightString.h"

class LSLPFire : public LSLightProgram {
protected:
	uint8_t colorIndex;
	uint8_t changeRate;

public:
	LSLPFire(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);
	void update();

};

LSLightProgram *factoryFire(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
