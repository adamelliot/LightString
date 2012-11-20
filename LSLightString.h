#ifndef _LSLIGHTSTRING_H_
#define _LSLIGHTSTRING_H_

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

// === Color Palette ===

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

class LSColorPalette {
protected:
	uint8_t flags;
	pcolor_func colorFunc;

public:
	LSColorPalette();
	
	void setColorFunc(pcolor_func colorFunc);
	pcolor_func getColorFunc();
	
	uint8_t getFlags();
	bool isMirrored();

	virtual void setConfig(void *config);
	virtual void *getConfig();
	
	virtual color_t getColor(uint8_t index);
};

// === Pixel Buffer ===

/**
 * Class managing pixels. General managing a LSLEDStrand
 */
class LSPixelBuffer {
private:

	uint16_t getIndex(uint8_t x, uint8_t y);

	uint16_t width, height;

	pcolor_t pixels;
	uint16_t length;
	
public:

	LSPixelBuffer(pcolor_t pixels, uint16_t length);

	color_t *getPixels();
	uint16_t getLength();
	
	void setSize(uint16_t width, uint16_t height);
	uint16_t getWidth();
	uint16_t getHeight();

	void setPixel(uint16_t index, color_t col);
	void setMirroredPixel(uint16_t index, color_t col);

	void setPixel(uint8_t x, uint8_t y, color_t col);
	
	color_t getPixel(uint16_t index);
	color_t getPixel(uint8_t x, uint8_t y);

	void drawColumn(uint8_t x, uint8_t y, uint8_t y1, color_t col);
	void drawRow(uint8_t x, uint8_t x1, uint8_t y, color_t col);
	void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, color_t col);

	void fade(uint8_t rShift = 1, uint8_t gShift = 1, uint8_t bShift = 1);
	void fade(float ratio);
	void clear();
	void clear(color_t col);
	
	void shiftUp(uint16_t by = 1);
	void shiftDown(uint16_t by = 1);
	
	void blur();

};

// === LED Strip ===

/**
 * A class for simple management of WS2801 LED strips.
 * Based off WS2801 from Adafruit.
 */
class LSLEDStrip {

private:

	void alloc(uint16_t n);

	pcolor_t pixels;
	uint16_t length;
	
	bool useHardwareSPI;
  uint8_t
    clkpin    , datapin,     // Clock & data pin numbers
    clkpinmask, datapinmask; // Clock & data PORT bitmasks
  volatile uint8_t *clkport, *dataport;   // Clock & data PORT registers

public:

	LSLEDStrip(uint16_t n, uint8_t dpin, uint8_t cpin);
	LSLEDStrip(uint16_t n);

	void updatePins(uint8_t dpin, uint8_t cpin); // Change pins, configurable
	void updatePins(void);

	void update(void);

	uint16_t getLength(void);
	LSPixelBuffer *getPixelBuffer(uint16_t length = 0, uint16_t offset = 0);

};

// === Light Program ===

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

// === Light Program Manager ===

#ifndef MAX_LIGHT_SECTIONS
#define MAX_LIGHT_SECTIONS 2
#endif

#ifndef MAX_LIGHT_PROGRAMS
#define MAX_LIGHT_PROGRAMS 12
#endif

#ifndef MAX_COLOR_PALETTES
#define MAX_COLOR_PALETTES 16
#endif

#define ALL_STRIPS 0xff

#define PROGRAM_LENGTH 10000 //

typedef LSLightProgram *(*plight_program_factory_func)(LSPixelBuffer *, LSColorPalette *, pcolor_func);

typedef struct color_palette_s color_palette_t, *pcolor_palette_t;
typedef struct light_program_s light_program_t, *plight_program_t;
typedef struct light_section_s light_section_t, *plight_section_t;

pcolor_palette_t create_color_palette(pcolor_palette_factory_func factory, void *config);
plight_program_t create_light_program(plight_program_factory_func factory, void *config);
plight_section_t create_light_section(pcolor_func colorFunc, LSLEDStrip *lightStrip, uint16_t length, uint16_t offset);

struct color_palette_s {
	pcolor_palette_factory_func factory;
	void *config;
};

struct light_program_s {
	plight_program_factory_func factory;
	void *config;
};

struct light_section_s {
	pcolor_func colorFunc;
	plight_program_t supportedPrograms[MAX_LIGHT_PROGRAMS];
	LSPixelBuffer *pixelBuffer;
	LSLightProgram *activeProgram;
};

class LSLightProgramManager {
private:
	uint8_t programs[MAX_LIGHT_PROGRAMS];
	pcolor_palette_t colorPalettes[MAX_COLOR_PALETTES];
	plight_section_t lightSections[MAX_LIGHT_SECTIONS];
	LSLEDStrip *lightStrips[MAX_LIGHT_SECTIONS];
	
	uint32_t programLength;
	
	uint8_t programsCount;
	uint8_t palettesCount;
	uint8_t lightSectionsCount;
	
	uint8_t paletteOrder[MAX_COLOR_PALETTES];
	uint8_t paletteIndex;
	
	uint8_t programOrder[MAX_LIGHT_PROGRAMS];
	uint8_t activeProgramID;
	
	LSColorPalette *activeColorPalette;
	
	uint32_t lastTime;
	uint32_t programSwitchAfter;
	uint16_t msPerFrame;
	
	void randomizePaletteOrder();

	void saveState();
	void loadState();

	void pulse();
	void fadeDown();

public:
	
	LSLightProgramManager(uint32_t programLength = PROGRAM_LENGTH);

	uint8_t addLightSection(pcolor_func colorFunc, LSLEDStrip *lightStrip, uint16_t length = 0, uint16_t offset = 0);
	void addLightProgram(uint8_t strips, uint8_t id, plight_program_factory_func factory, void *config = NULL);
	void addColorPalette(pcolor_palette_factory_func factory, void *config = NULL);

	void selectNewProgram(uint8_t id = MAX_LIGHT_PROGRAMS);
	
	void loop();
};

// Palettes
#include "LSCPGradientPalette.h"

// Programs
#include "LSLPRainbowCycle.h"
#include "LSLPTwinkle.h"
#include "LSLPSolidCycle.h"
#include "LSLPBounce.h"
#include "LSLPPulse.h"

#include "LSLP2DLines.h"
#include "LSLPFire.h"
#include "LSLPPlasma.h"
#include "LSLPBlit.h"

#endif
