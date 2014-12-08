#ifndef _LSPIXELBUFFER_H_
#define _LSPIXELBUFFER_H_

#include "FastLED.h"
#include "palette.h"

/**
 * Class managing pixels. General managing a LSLEDStrand
 */
class LSPixelBuffer {
private:

	uint16_t width, height;
	uint16_t length;

	CRGB *pixels;

	uint16_t getIndex(uint8_t x, uint8_t y);

public:

	LSPixelBuffer(CRGB *pixels = 0, uint16_t length = 0);

	void setPixels(CRGB *pixels, uint16_t length);
	CRGB *getPixels();
	uint16_t getLength();
	
	void setSize(uint16_t width, uint16_t height);
	uint16_t getWidth();
	uint16_t getHeight();

	void setPixel(uint16_t index, CRGB col);
	void setMirroredPixel(uint16_t index, CRGB col);
	void setPixelAt(uint8_t x, uint8_t y, CRGB col);

	// Return a pointer to the pixel
	CRGB getPixel(uint16_t index);
	CRGB getPixelAt(uint8_t x, uint8_t y);

	void fade(uint8_t shift = 1);
	void fade(float ratio);
	void clear();
	void clear(CRGB col);

	void shiftLeft();
	void shiftUp(uint16_t by = 1);
	void shiftDown(uint16_t by = 1);
	
	void blur();
	
//	void drawText(uint8_t x, uint8_t y, uint8_t textX, uint8_t textY, uint8_t width, uint8_t height, const char *str);

	void drawColumn(uint8_t x, uint8_t y, uint8_t height, uint8_t *pixels);
	void drawColumn(uint8_t x, uint8_t y, uint8_t height, uint8_t colIndex);
	void drawColumn(uint8_t x, uint8_t y0, uint8_t y1, CRGB col);
	
	void drawRow(uint8_t x, uint8_t x1, uint8_t y, CRGB col);
	void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, CRGB col);

};

#endif