#ifndef _LSLP2DLINES_H_
#define _LSLP2DLINES_H_

#include "LSLightString.h"

class LSLP2DLines : public LSLightProgram {
protected:
	bool fade;

	float fadeRate;
	uint8_t colorIndex;
	uint8_t changeRate;
	
	float x0, y0, sx0, sy0;
	float x1, y1, sx1, sy1;
	float x2, y2, sx2, sy2;
	float x3, y3, sx3, sy3;

public:
	LSLP2DLines(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);
	void update();

};

LSLightProgram *factory2DLines(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
