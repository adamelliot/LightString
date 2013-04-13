#ifndef _LSLIGHTPROGRAM_H_
#define _LSLIGHTPROGRAM_H_

#include "LSPixelBuffer.h"
#include "LSZXSound.h"

typedef LSColorPalette *(*pcolor_palette_factory_func)();

class LSLightProgram {
protected:
	pcolor_func colorFunc;
	LSColorPalette *colorPalette;
	LSPixelBuffer *pixelBuffer;
	LSZXSound *zxSound;

public:
	LSLightProgram(LSPixelBuffer *pixelBuffer, LSColorPalette *colorPalette, pcolor_func colorFunc);

	LSPixelBuffer *getPixelBuffer();

	void setColorFunc(pcolor_func colorFunc);
	pcolor_func getColorFunc();
	
	void setZXSound(LSZXSound *zxSound);

	virtual uint8_t getFrameRate();

	virtual void setConfig(void *config);
	virtual void *getConfig();

	virtual void update();
};

#endif
