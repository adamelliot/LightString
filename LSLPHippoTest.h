#ifndef _LSLPHIPPOTEST_H_
#define _LSLPHIPPOTEST_H_

#include "LSLightString.h"

class LSLPHippoTest : public LSLightProgram {
protected:
	uint8_t colorIndex;
	uint8_t changeRate;

public:
	LSLPHippoTest(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);
	void update();

};

LSLightProgram *factoryHippoTest(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
