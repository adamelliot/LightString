#ifndef _LSCOLORPALETTE_H_
#define _LSCOLORPALETTE_H_

#include <Arduino.h>

typedef struct color_s color_t, *pcolor_t;

typedef struct colorRGB_s colorRGB_s, *pcolorRGB_t;
typedef struct colorRBG_s colorRBG_s, *pcolorRBG_t;
typedef struct colorBGR_s colorBGR_s, *pcolorBGR_t;

typedef color_t (*pcolor_func)(uint8_t r, uint8_t g, uint8_t b);

struct color_s {
	uint8_t channels[3];
};

struct colorRGB_s {
	uint8_t r, g, b;
};

struct colorRBG_s {
	uint8_t r, b, g;
};

struct colorBGR_s {
	uint8_t g, b, r;
};

color_t colorRGB(uint8_t r, uint8_t g, uint8_t b);
color_t colorRBG(uint8_t r, uint8_t g, uint8_t b);
color_t colorBGR(uint8_t r, uint8_t g, uint8_t b);

color_t randomColor();
color_t whiteColor();
color_t blackColor();

#define PALETTE_MIRRORED 0x01
#define PALETTE_SIZE 256 // 3 colors per pixel total bytes = 768

class LSColorPalette {
protected:
	uint8_t flags;
	pcolor_func colorFunc;
	
	pcolor_t paletteTable;

public:
	LSColorPalette();
	~LSColorPalette();
	
	void setColorFunc(pcolor_func colorFunc);
	pcolor_func getColorFunc();
	
	uint8_t getFlags();
	bool isMirrored();
	
	pcolor_t generatePaletteTable();
	void freePaletteTable();

	pcolor_t getPaletteTable();
	
	void fade(uint8_t shift = 1);
	void fade(float ratio);

	virtual bool usePaletteTable();

	virtual void setConfig(void *config);
	virtual void *getConfig();
	
	virtual color_t getColor(uint8_t index);
	virtual color_t getTableColor(uint8_t index);
};

#endif