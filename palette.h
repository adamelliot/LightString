#ifndef _PALETTE_H_
#define _PALETTE_H_

#include <Arduino.h>
#include <EEPROM.h>
#include <FastLED.h>

#define PALETTE_MIRRORED 0x01
#define PALETTE_SIZE 256

void printColor(CRGB col);

#define MAX_PALETTE_COLORS 6

#define LOG(MSG, VAL) {Serial.print(F(MSG " ")); Serial.println(VAL); }
#define LOG2(MSG, V1, V2) {Serial.print(F(MSG " ")); Serial.print(V1); Serial.print(F(", ")); Serial.println(V2); }

namespace LightString {

struct CRGBPalette {
	uint8_t size;
	CRGB colors[MAX_PALETTE_COLORS];

	inline CRGB operator[] (uint16_t index) __attribute__((always_inline))
	{
		uint16_t totalColors = PALETTE_SIZE;
	
		uint16_t indexSections = index * (size - 1);
		uint8_t section = indexSections / totalColors;
		uint32_t weight = indexSections % totalColors;

		CRGB col = colors[section];

		return col.lerp8(colors[section + 1], weight);
	}

	/*
	inline CRGB operator[] (uint16_t index) const __attribute__((always_inline))
	{
		uint16_t totalColors = PALETTE_SIZE;
	
		uint16_t indexSections = index * (size - 1);
		uint8_t section = indexSections / totalColors;
		uint32_t weight = indexSections % totalColors;

		CRGB col = colors[section];
		return col.lerp8(colors[section + 1], weight);
	}*/

	inline CRGBPalette() : size(3) {
		colors[0] = CRGB::Red;
		colors[1] = CRGB::Green;
		colors[2] = CRGB::Blue;
	}

	inline CRGBPalette(uint8_t len, const CRGB newColors[], bool mirrored = false)
		: size(mirrored ? ((len << 1) - 1) : len)
	{
		memcpy8(colors, newColors, len * sizeof(CRGB));

		if (mirrored) {
			uint8_t end = size - 1;
			for (uint8_t i = end; i >= len; i--) {
				colors[i] = colors[end - i];
			}
		}
	}
	
	inline CRGBPalette(CRGB col0) : size(1)
	{ colors[0] = col0; }

	inline CRGBPalette(CRGB col0, CRGB col1) : size(2)
	{ colors[0] = col0; colors[1] = col1; }

	inline CRGBPalette(CRGB col0, CRGB col1, CRGB col2) : size(3)
	{ colors[0] = col0; colors[1] = col1; colors[2] = col2; }

	inline CRGBPalette(CRGB col0, CRGB col1, CRGB col2, CRGB col3) : size(4)
	{ colors[0] = col0; colors[1] = col1; colors[2] = col2; colors[3] = col3; }

	inline CRGBPalette(CRGB col0, CRGB col1, CRGB col2, CRGB col3, CRGB col4) : size(5)
	{ colors[0] = col0; colors[1] = col1; colors[2] = col2; colors[3] = col3; colors[4] = col4; }

	inline CRGBPalette& operator= (const CRGBPalette& rhs) __attribute__((always_inline))
	{
		if (size != rhs.size) {
			size = rhs.size;
		}

		memcpy8(colors, rhs.colors, size * sizeof(CRGB));

    return *this;
	}
	
	inline CRGBPalette& nscale8(uint8_t scale)
	{
		for (uint8_t i = 0; i < size; i++) {
			colors[i].nscale8(scale);
		}
    
    return *this;
	}
};

#define SOLID_RED										CRGBPalette(CRGB::Red, CRGB::Red)
#define SOLID_GREEN									CRGBPalette(CRGB::Lime, CRGB::Lime)
#define SOLID_BLUE									CRGBPalette(CRGB::Blue, CRGB::Blue)
#define SOLID_AQUA									CRGBPalette(CRGB::Aqua, CRGB::Aqua)
#define SOLID_AZURE									CRGBPalette(CRGB::Azure, CRGB::Azure)

#define RAINBOW_GRADIENT 						CRGBPalette(CRGB::Red, CRGB::Lime, CRGB::Blue, CRGB::Red)

#define BLUE_GREEN_GRADIENT 				CRGBPalette(CRGB::Blue, CRGB(0, 255, 64), CRGB::Blue)
#define GREEN_GRADIENT 							CRGBPalette(CRGB::Lime, CRGB(0, 32, 0), CRGB::Lime)
#define LIME_GRADIENT 							CRGBPalette(CRGB::Lime, CRGB::Green, CRGB::Lime)
#define YELLOW_GRADIENT 						CRGBPalette(CRGB(255, 191, 63), CRGB(63, 47, 15), CRGB(255, 191, 63))
#define WHITE_GRADIENT 							CRGBPalette(CRGB::White, CRGB::Black, CRGB::White)

#define RED_GREEN_GRADIENT 					CRGBPalette(CRGB::Red, CRGB::Green)

#define GREEN_BLUE_GRADIENT 				CRGBPalette(CRGB(0, 255, 64), CRGB(0, 0, 255), CRGB(0, 255, 64))
#define RED_ORGANGE_GRADIENT 				CRGBPalette(CRGB(128, 128, 128), CRGB(128, 31, 0), CRGB(128, 95, 0), CRGB(128, 31, 0), CRGB(128, 128, 128))

#define BLUE_WHITE_GRADIENT 				CRGBPalette(CRGB(0, 31, 255), CRGB(255, 255, 255), CRGB(0, 31, 255))
#define BLUE_WHITISH_GRADIENT				CRGBPalette(CRGB(0, 31, 255), CRGB(191, 191, 191), CRGB(0, 31, 255))

#define BLUE_WHITE_YELLOW_GRADIENT 	CRGBPalette(CRGB(0, 95, 255), CRGB(255, 255, 0), CRGB::White, CRGB(0, 31, 255))


#define BLUE_YELLOW_BLACK_GRADIENT 	CRGBPalette(CRGB(0, 95, 255), CRGB(255, 255, 0), CRGB::White, CRGB::Black, CRGB(0, 31, 255))
#define RED_WHITE_BLACK_GRADIENT 		CRGBPalette(CRGB(255, 0, 31), CRGB(255, 0, 0), CRGB::White, CRGB::Black, CRGB(255, 0, 31))

#define RED_WHITE_GRADIENT 					CRGBPalette(CRGB(255, 31, 0), CRGB(255, 0, 0), CRGB(255, 255, 255), CRGB(255, 31, 0))
#define GREEN_WHITE_GRADIENT 				CRGBPalette(CRGB(0, 255, 31), CRGB(0, 255, 0), CRGB(255, 255, 255), CRGB(0, 255, 31))
#define RED_WHITISH_GRADIENT 				CRGBPalette(CRGB(255, 31, 0), CRGB(255, 0, 0), CRGB(191, 191, 191), CRGB(255, 31, 0))

#define CYAN_PINK_GRADIENT 					CRGBPalette(CRGB(0, 255, 255), CRGB(255, 0, 96), CRGB(0, 255, 255))
#define BLUE_YELLOW_GRADIENT 				CRGBPalette(CRGB(0, 95, 255), CRGB(255, 255, 0), CRGB(0, 31, 255))
#define GREEN_YELLOW_GRADIENT 			CRGBPalette(CRGB(31, 255, 31), CRGB(255, 191, 63), CRGB(31, 255, 31))
#define RAINBOW_BLACK_GRADIENT 			CRGBPalette(CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black, CRGB::Red)

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


#define CHANNEL_SIZE 3
#define CUSTOM_PALETTE_SIZE (1 + (CHANNEL_SIZE * 8))
#define EEPROM_OFFSET 32

class PaletteManager {
protected:
	bool useEEPROM;
	bool writePalettes;

	uint8_t staticPalettes;
	uint8_t customPalettes;

	uint16_t staticOffset;
	
	uint8_t paletteIndex;
//	uint8_t *paletteOrder;

	CRGBPalette currentPalette;

	void resetCustomPalettes();
	void writePalette(uint16_t offset, CRGBPalette &palette);
	CRGBPalette readPalette(uint16_t offset);
	
public:

	PaletteManager();

	void updateEEPROM();
	void randomizeOrder();

	uint8_t paletteCount();

	void setCustomPalettes(uint8_t customPalettes, bool reset = false);
	
	CRGB getColor(uint8_t index);
	CRGBPalette getPalette() { return currentPalette; }

	void add(CRGBPalette palette);
	void setActive(CRGBPalette &palette);
	CRGBPalette &getActive() { return currentPalette; }

	void loadPalette(uint8_t index);
	
	void next();
	void previous();
};

};

extern LightString::PaletteManager Palettes;

#endif