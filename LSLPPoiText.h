#ifndef _LSLPPOITEXT_H_
#define _LSLPPOITEXT_H_

#include "LSLightString.h"

class LSLPPoiText : public LSLightProgram {
protected:
	uint8_t colorIndex;
	uint8_t indexStep;

	char *message;

public:
	LSLPPoiText(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);
	
	uint8_t getFrameRate();
	void update();
};

LSLightProgram *factoryPoiText(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
