#ifndef _LSLPPLASMA_H_
#define _LSLPPLASMA_H_

#include "LSLightString.h"

class LSLPPlasma : public LSLightProgram {
protected:
	uint8_t colorIndex;
	uint8_t changeRate;

public:
	LSLPPlasma(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);
	void update();

};

LSLightProgram *factoryPlasma(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
