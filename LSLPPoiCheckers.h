#ifndef _LSLPPOICHECKERS_H_
#define _LSLPPOICHECKERS_H_

#include "LSLightString.h"

class LSLPPoiCheckers : public LSLightProgram {
protected:
	uint8_t switchIndex;
	uint8_t colorIndex;
	uint8_t switcher;
	uint8_t colorStep;
	
public:
	LSLPPoiCheckers(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);
	
	uint8_t getFrameRate();
	void update();
};

LSLightProgram *factoryPoiCheckers(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
