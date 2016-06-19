#ifndef _PALETTE_H_
#define _PALETTE_H_

#include <stdint.h>
#include <stdio.h>
#include "colortypes.h"

// #include "drawing.h"

#ifndef PROGMEM
#define PROGMEM
#endif

const uint16_t kPaletteSize = 256;

#define MAX_PALETTE_COLORS 12

#define LOG(MSG, VAL) {Serial.print(F(MSG " ")); Serial.println(VAL); }
#define LOG2(MSG, V1, V2) {Serial.print(F(MSG " ")); Serial.print(V1); Serial.print(F(", ")); Serial.println(V2); }

namespace LightString {

void printColor(RGBu col);

struct IPalette {
private:
public:
};

template<typename PIXEL>
struct TPalette : IPalette {
	uint8_t size;
	PIXEL colors[MAX_PALETTE_COLORS];

	TPalette(const TPalette<PIXEL> &rhs) {
		for (int i = 0; i < rhs.size; i++) {
			this->colors[i] = rhs.colors[i];
		}
		this->size = rhs.size;
	}

	inline PIXEL operator[] (uint8_t index) __attribute__((always_inline))
	{
		uint16_t indexSections = index * (size - 1);
		uint8_t section = indexSections / kPaletteSize;
		uint32_t weight = indexSections % kPaletteSize;

		PIXEL col = colors[section];

		return col.lerp8(colors[(section + 1) % size], weight);
	}
	
	TPalette<PIXEL>& operator= (const TPalette<RGBu> &rhs) {
		for (int i = 0; i < rhs.size; i++) {
			this->colors[i] = rhs.colors[i];
		}
		this->size = rhs.size;
		return *this;
	}

	TPalette<PIXEL>& operator= (const TPalette<RGBAu> &rhs) {
		for (int i = 0; i < rhs.size; i++) {
			this->colors[i] = rhs.colors[i];
		}
		this->size = rhs.size;
		return *this;
	}

	/*
	inline PIXEL operator[] (uint16_t index) const __attribute__((always_inline))
	{
		uint16_t totalColors = PALETTE_SIZE;
	
		uint16_t indexSections = index * (size - 1);
		uint8_t section = indexSections / totalColors;
		uint32_t weight = indexSections % totalColors;

		PIXEL col = colors[section];
		return col.lerp8(colors[section + 1], weight);
	}*/

	inline TPalette() : size(3) {
		colors[0] = RGBAu(255, 0, 0);
		colors[1] = RGBAu(255, 0, 0);
		colors[2] = RGBAu(255, 0, 0);
	}

	inline TPalette(uint8_t len, const PIXEL newColors[], bool mirrored = false)
		: size(mirrored ? ((len << 1) - 1) : len)
	{
		memcpy8(colors, newColors, len * sizeof(PIXEL));

		if (mirrored) {
			uint8_t end = size - 1;
			for (uint8_t i = end; i >= len; i--) {
				colors[i] = colors[end - i];
			}
		}
	}
	
	inline TPalette(PIXEL col0) : size(1)
	{ colors[0] = col0; }

	inline TPalette(PIXEL col0, PIXEL col1) : size(2)
	{ colors[0] = col0; colors[1] = col1; }

	inline TPalette(PIXEL col0, PIXEL col1, PIXEL col2) : size(3)
	{ colors[0] = col0; colors[1] = col1; colors[2] = col2; }

	inline TPalette(PIXEL col0, PIXEL col1, PIXEL col2, PIXEL col3) : size(4)
	{ colors[0] = col0; colors[1] = col1; colors[2] = col2; colors[3] = col3; }

	inline TPalette(PIXEL col0, PIXEL col1, PIXEL col2, PIXEL col3, PIXEL col4) : size(5)
	{ colors[0] = col0; colors[1] = col1; colors[2] = col2; colors[3] = col3; colors[4] = col4; }
	
	inline TPalette(PIXEL col0, PIXEL col1, PIXEL col2, PIXEL col3, PIXEL col4, PIXEL col5) : size(6)
	{ colors[0] = col0; colors[1] = col1; colors[2] = col2; colors[3] = col3; colors[4] = col4; colors[5] = col5; }

	inline TPalette(PIXEL col0, PIXEL col1, PIXEL col2, PIXEL col3, PIXEL col4, PIXEL col5, PIXEL col6) : size(7)
	{ colors[0] = col0; colors[1] = col1; colors[2] = col2; colors[3] = col3; colors[4] = col4; colors[5] = col5; colors[6] = col6; }
	/*
	inline TPalette& operator= (const TPalette<PIXEL>& rhs)
	{
		if (size != rhs.size) {
			size = rhs.size;
		}

		memcpy8(colors, rhs.colors, size * sizeof(PIXEL));

    return *this;
	}*/
	
	inline TPalette& nscale8(uint8_t scale)
	{
		for (uint8_t i = 0; i < size; i++) {
			colors[i].nscale8(scale);
		}

		return *this;
	}
};

typedef TPalette<RGBu> CRGBPalette;

//TPalette<RGBA> &testPalette() {
//	static TPalette<RGBA> pal(CRGB(0, 255, 64), CRGB(0, 0, 255), CRGB(0, 255, 64));
//	return pal;
//}

extern TPalette<RGBAu> PROGMEM SOLID_RED										;
extern TPalette<RGBAu> PROGMEM SOLID_GREEN									;
extern TPalette<RGBAu> PROGMEM SOLID_BLUE									;
extern TPalette<RGBAu> PROGMEM SOLID_AQUA									;
extern TPalette<RGBAu> PROGMEM SOLID_AZURE									;
extern TPalette<RGBAu> PROGMEM RAINBOW_GRADIENT 						;
extern TPalette<RGBAu> PROGMEM BLUE_GREEN_GRADIENT 				;
extern TPalette<RGBAu> PROGMEM GREEN_GRADIENT 							;
extern TPalette<RGBAu> PROGMEM LIME_GRADIENT 							;
extern TPalette<RGBAu> PROGMEM YELLOW_GRADIENT 						;
extern TPalette<RGBAu> PROGMEM WHITE_GRADIENT 							;
extern TPalette<RGBAu> PROGMEM RED_GREEN_GRADIENT 					;
extern TPalette<RGBAu> PROGMEM GREEN_BLUE_GRADIENT 				;
extern TPalette<RGBAu> PROGMEM RED_ORGANGE_GRADIENT 				;
extern TPalette<RGBAu> PROGMEM BLUE_WHITE_GRADIENT 				;
extern TPalette<RGBAu> PROGMEM BLUE_WHITISH_GRADIENT				;
extern TPalette<RGBAu> PROGMEM BLUE_WHITE_YELLOW_GRADIENT 	;
extern TPalette<RGBAu> PROGMEM BLUE_YELLOW_BLACK_GRADIENT 	;
extern TPalette<RGBAu> PROGMEM RED_WHITE_BLACK_GRADIENT 		;
extern TPalette<RGBAu> PROGMEM RED_WHITE_GRADIENT 					;
extern TPalette<RGBAu> PROGMEM GREEN_WHITE_GRADIENT 				;
extern TPalette<RGBAu> PROGMEM RED_WHITISH_GRADIENT 				;
extern TPalette<RGBAu> PROGMEM CYAN_PINK_GRADIENT 					;
extern TPalette<RGBAu> PROGMEM BLUE_YELLOW_GRADIENT 				;
extern TPalette<RGBAu> PROGMEM GREEN_YELLOW_GRADIENT 			;
extern TPalette<RGBAu> PROGMEM RAINBOW_BLACK_GRADIENT 			;

// Palettes

/*

PALLETE_2_M(pinkPurpleGradient, CRGB(255, 0, 128), CRGB(96, 0, 192));
PALLETE_2_M(solidRedGradient, CRGB(255, 192, 0), CRGB(255, 192, 0));

PALLETE_5(pinkPurpleBlackGradient, CRGB(255, 0, 128), CRGB(96, 0, 192), CRGB::Black, CRGB::Black, CRGB(255, 0, 128));

PALLETE_5(cyanPinkBlackGradient, CRGB(0, 255, 192), CRGB::Black, CRGB(255, 16, 64), CRGB::Black, CRGB(0, 255, 192));

PALLETE_5(greenBlueBlackGradient, CRGB(0, 255, 64), CRGB(0, 0, 255), CRGB::Black, CRGB::Black, CRGB(0, 255, 64));

PALLETE_5(redOrgangeBlackGradient, CRGB(255, 31, 0), CRGB(255, 191, 0), CRGB::Black, CRGB::Black, CRGB(255, 31, 0));

PALLETE_5(blueWhiteBlackGradient, CRGB(0, 31, 25), CRGB(255, 255, 255), CRGB::White, CRGB::Black, CRGB(0, 31, 255));
PALLETE_4(blueYellowBlackHardGradient, CRGB(0, 0, 0), CRGB(16, 16, 0), CRGB(255, 255, 0), CRGB(0, 95, 255));

PALLETE_2(whiteSolidGradient, CRGB(64, 64, 64), CRGB(64, 64, 64));
*/

template<size_t MAX_SWATCHES, typename PIXEL>
class SwatchManager {
protected:
	PIXEL swatches[MAX_SWATCHES];
	uint8_t swatchCount;
	uint8_t swatchIndex;

public:

	SwatchManager() : swatchCount(0), swatchIndex(0) {
		swatchCount = 0;
		swatchIndex = 0;
	}

	void shuffle();
	uint8_t getSwatchCount() { return swatchCount; }

	PIXEL getColor() { return swatches[swatchIndex]; }
	TPalette<PIXEL> generatePalette(uint8_t colorStops, uint8_t padding = 1, bool mirrored = true);
	void add(PIXEL color);
	
	PIXEL next();
	PIXEL previous();
};

template<size_t MAX_PALETTES, typename PIXEL>
class PaletteManager {
protected:
	TPalette<PIXEL> *palettes[MAX_PALETTES];
	uint8_t paletteCount;
	uint8_t paletteIndex;

public:

	PaletteManager() : paletteCount(0), paletteIndex(0) {}

	void shuffle();
	uint8_t getPaletteCount() { return paletteCount; }

	PIXEL getColor(uint8_t index) { return (*palettes[paletteIndex])[index]; }
	inline TPalette<PIXEL> getPalette() { return *palettes[paletteIndex]; }

	void loadPalette(uint8_t index) { paletteIndex = index; }

	void add(TPalette<PIXEL> &palette);
	
	void next();
	void previous();
};

#include "palette.tpp"

};

extern LightString::SwatchManager<10, LightString::RGBAu> Swatches;
extern LightString::PaletteManager<20, LightString::RGBAu> Palettes;

#endif
