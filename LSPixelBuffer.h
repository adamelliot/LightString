#ifndef _LSPIXELBUFFER_H_
#define _LSPIXELBUFFER_H_

#include "LSColorPalette.h"

#define INDEXED_PIXEL_BUFFER 0x01

/**
 * Class managing pixels. General managing a LSLEDStrand
 */
class LSPixelBuffer {
private:
	
	typedef void (LSPixelBuffer::*psetIndexedPixel)(uint16_t index, uint8_t colIndex);
	typedef void (LSPixelBuffer::*psetIndexedPixelAt)(uint8_t x, uint8_t y, uint8_t colIndex);

	LSColorPalette *colorPalette;

	uint8_t flags;
	uint8_t pixelBytes;
	uint16_t width, height;
	uint16_t length, bytes;

	void *pixels;

	uint16_t getIndex(uint8_t x, uint8_t y);

public:

	LSPixelBuffer(void *pixels, uint16_t length, uint8_t flags = 0);

	bool useIndexedPixelBuffer();

	void *getPixels();
	uint16_t getLength();
	
	void setSize(uint16_t width, uint16_t height);
	uint16_t getWidth();
	uint16_t getHeight();
	
	LSColorPalette *getColorPalette(void);
	void setColorPalette(LSColorPalette *colorPalette);

	void setPixel(uint16_t index, color_t col);
	void setMirroredPixel(uint16_t index, color_t col);
	void setPixelAt(uint8_t x, uint8_t y, color_t col);

	psetIndexedPixel setIndexedPixel;
	psetIndexedPixel setMirroredIndexedPixel;
	psetIndexedPixelAt setIndexedPixelAt;

	void setPixelWithColorIndex(uint16_t index, uint8_t colIndex);
	void setPixelWithPaletteIndex(uint16_t index, uint8_t colIndex);

	void setMirroredPixelWithColorIndex(uint16_t index, uint8_t colIndex);
	void setMirroredPixelWithPaletteIndex(uint16_t index, uint8_t colIndex);

	void setPixelWithColorIndexAt(uint8_t x, uint8_t y, uint8_t colIndex);
	void setPixelWithPaletteIndexAt(uint8_t x, uint8_t y, uint8_t colIndex);

	// Return a pointer to the pixel
	void *getPixel(uint16_t index);
	void *getPixelAt(uint8_t x, uint8_t y);

	void fade(uint8_t shift = 1);
	void fade(float ratio);
	void clear();
	void clear(color_t col);

	void shiftLeft();
	void shiftUp(uint16_t by = 1);
	void shiftDown(uint16_t by = 1);
	
	void blur();
	
	void drawText(uint8_t x, uint8_t y, uint8_t textX, uint8_t textY, uint8_t width, uint8_t height, const char *str);

	void drawColumn(uint8_t x, uint8_t y, uint8_t height, uint8_t *pixels);
	void drawColumn(uint8_t x, uint8_t y, uint8_t height, uint8_t colIndex);
	void drawColumn(uint8_t x, uint8_t y0, uint8_t y1, color_t col);
	
	void drawRow(uint8_t x, uint8_t x1, uint8_t y, color_t col);
	void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, color_t col);

};

#endif