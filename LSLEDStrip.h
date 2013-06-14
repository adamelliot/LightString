#ifndef _LSLEDSTRIP_H_
#define _LSLEDSTRIP_H_

#include "LSPixelBuffer.h"
#include "FastSPI_LED2.h"

/**
 * A wrapper for FastSPI2 CLEDController to bridge the frameworks
 */
class LSLEDStrip {

private:
	
	CLEDController *controller;
	LSColorPalette *colorPalette;

	void *pixels;
	uint16_t length;

public:

	LSLEDStrip(CLEDController *controller, uint16_t length);

	void *getPixels(void);
	
	LSColorPalette *getColorPalette(void);
	void setColorPalette(LSColorPalette *colorPalette);

	uint16_t getLength(void);
	LSPixelBuffer *getPixelBuffer(uint16_t length = 0, uint16_t offset = 0);

	void update();
};

#endif