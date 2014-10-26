#ifndef _LSCOLORPALETTE_H_
#define _LSCOLORPALETTE_H_

#include <Arduino.h>
#include <FastLED.h>

void printColor(CRGB col);

CRGB randomColor();

#define PALETTE_MIRRORED 0x01
#define PALETTE_SIZE 256 // 3 colors per pixel total bytes = 768

class LSColorPalette {
protected:
	uint8_t flags;
	
	CRGB *paletteTable;

public:
	LSColorPalette();
	~LSColorPalette();
	
	uint8_t getFlags();
	bool isMirrored();
	
	CRGB *generatePaletteTable();
	void freePaletteTable();

	CRGB *getPaletteTable();

	void fade(uint8_t shift = 1);
	void fade(float ratio);

	virtual bool usePaletteTable();

	virtual void setConfig(void *config);
	virtual void *getConfig();
	
	virtual CRGB getColor(uint8_t index);
	virtual CRGB getTableColor(uint8_t index);
};

#endif