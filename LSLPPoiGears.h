#ifndef _LSLPPOIGEARS_H_
#define _LSLPPOIGEARS_H_

#include "LSLightString.h"

class LSLPPoiGears : public LSLightProgram {
protected:
	uint8_t switchIndex;
	uint8_t colorIndex;
	uint8_t switcher;
	uint8_t colorStep;
	
public:
	LSLPPoiGears(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);
	
	uint8_t getFrameRate();
	void update();
};

LSLightProgram *factoryPoiGears(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
