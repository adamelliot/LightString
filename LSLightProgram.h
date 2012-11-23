#ifndef _LSLIGHTPROGRAM_H_
#define _LSLIGHTPROGRAM_H_

#include "LSPixelBuffer.h"

typedef LSColorPalette *(*pcolor_palette_factory_func)();

class LSLightProgram {
protected:
	pcolor_func colorFunc;
	LSColorPalette *colorPalette;
	LSPixelBuffer *pixelBuffer;

public:
	LSLightProgram(LSPixelBuffer *pixelBuffer, LSColorPalette *colorPalette, pcolor_func colorFunc);

	LSPixelBuffer *getPixelBuffer();

	void setColorFunc(pcolor_func colorFunc);
	pcolor_func getColorFunc();

	virtual uint8_t getFrameRate();

	virtual void setConfig(void *config);
	virtual void *getConfig();

	virtual void update();
};

#endif
