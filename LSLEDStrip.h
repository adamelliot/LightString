#ifndef _LSLEDSTRIP_H_
#define _LSLEDSTRIP_H_

#include "LSPixelBuffer.h"

/**
 * A class for simple management of WS2801 LED strips.
 * Based off WS2801 from Adafruit.
 */
class LSLEDStrip {

private:

	typedef void (LSLEDStrip::*pupdateFunc)(void);

	void alloc(uint16_t n);
	
	uint8_t flags;

	LSColorPalette *colorPalette;

	void *pixels;
	uint16_t length;
	
	bool useHardwareSPI;
  uint8_t
    clkpin    , datapin,     // Clock & data pin numbers
    clkpinmask, datapinmask; // Clock & data PORT bitmasks
  volatile uint8_t *clkport, *dataport;   // Clock & data PORT registers

	void updatePixels();
	void updateIndexedPixels();
	void updatePixelsWithSPI();
	void updateIndexedPixelsWithSPI();

public:

	LSLEDStrip(uint16_t n, uint8_t dpin, uint8_t cpin, uint8_t flags = 0);
	LSLEDStrip(uint16_t n, uint8_t flags = 0);

	void updatePins(uint8_t dpin, uint8_t cpin); // Change pins, configurable
	void updatePins(void);

	void *getPixels(void);

	LSColorPalette *getColorPalette(void);
	void setColorPalette(LSColorPalette *colorPalette);

	bool useIndexedPixelBuffer();

	uint16_t getLength(void);
	LSPixelBuffer *getPixelBuffer(uint16_t length = 0, uint16_t offset = 0);

	pupdateFunc updateFunc;

};

#endif