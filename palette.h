#ifndef _PALETTE_H_
#define _PALETTE_H_

#include <Arduino.h>
#include "FastLED.h"

#define PALETTE_MIRRORED 0x01
#define PALETTE_SIZE 256

void printColor(CRGB col);

#define MAX_PALETTE_COLORS 6

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
		//col = col.lerp8(colors[section + 1], weight);

		return col.lerp8(colors[section + 1], weight);
	}
	/*
	inline CRGB& operator[] (uint16_t index) const __attribute__((always_inline))
	{
		uint16_t totalColors = PALETTE_SIZE;
	
		uint16_t indexSections = index * (size - 1);
		uint8_t section = indexSections / totalColors;
		uint32_t weight = indexSections % totalColors;

		CRGB col = colors[section];
		col = col.lerp8(colors[section + 1], weight);
		
		return col;
	}*/
	
	inline CRGBPalette() : size(3) {
//		colors = (CRGB *)malloc(size * sizeof(CRGB));
//		LOG("malloc: ", (uint16_t)colors);

		colors[0] = CRGB::Red;
		colors[1] = CRGB::Green;
		colors[2] = CRGB::Blue;
	}

	inline CRGBPalette(uint8_t len, const CRGB newColors[], bool mirrored = false)
		: size(mirrored ? ((len << 1) - 1) : len)
	{
//		colors = (CRGB *)malloc(size * sizeof(CRGB));
//		LOG("malloc: ", (uint16_t)colors);
		// LOG("calloc: ", sizeof(CRGB) * size);
		memcpy8(colors, newColors, len * sizeof(CRGB));

		if (mirrored) {
			uint8_t end = size - 1;
			for (uint8_t i = end; i >= len; i--) {
				colors[i] = colors[end - i];
			}
		}
	}

	inline CRGBPalette& operator= (const CRGBPalette& rhs) __attribute__((always_inline))
	{
		if (size != rhs.size) {
//			free(colors);
//			colors = (CRGB *)realloc(colors, size * sizeof(CRGB));
			size = rhs.size;
//			LOG2("realloc: ", (uint16_t)colors, size * sizeof(CRGB));
		}

		memcpy8(colors, rhs.colors, size * sizeof(CRGB));
	}
	
	~CRGBPalette() {
		//LOG("free: ", sizeof(CRGB) * size);
//		LOG("free: ", (uint16_t)colors);
		free(colors);
	}
	
	inline CRGBPalette& nscale8(uint8_t scale)
	{
		for (uint8_t i = 0; i < size; i++) {
			colors[i].nscale8(scale);
		}
	}
};

#define RAINBOW_GRADIENT CRGBPalette(4, (CRGB[]){CRGB::Red, CRGB::Lime, CRGB::Blue, CRGB::Red})

#define BLUE_GREEN_GRADIENT CRGBPalette(2, (CRGB[]){CRGB::Blue, CRGB(0, 255, 64)}, true)
#define GREEN_GRADIENT CRGBPalette(2, (CRGB[]){CRGB::Green, CRGB(0, 32, 0)}, true)
#define YELLOW_GRADIENT CRGBPalette(2, (CRGB[]){CRGB(255, 191, 63), CRGB(63, 47, 15)}, true)
#define WHITE_GRADIENT CRGBPalette(2, (CRGB[]){CRGB::White, CRGB::Black}, true)

#define RED_GREEN_GRADIENT CRGBPalette(2, (CRGB[]){CRGB::Red, CRGB::Lime})

#define GREEN_BLUE_GRADIENT CRGBPalette(3, (CRGB[]){CRGB(0, 255, 64), CRGB(0, 0, 255), CRGB(0, 255, 64)})
#define RED_ORGANGE_GRADIENT CRGBPalette(3, (CRGB[]){CRGB(128, 128, 128), CRGB(128, 31, 0), CRGB(128, 95, 0)}, true)

#define BLUE_WHITE_GRADIENT CRGBPalette(3, (CRGB[]){CRGB(0, 31, 255), CRGB(255, 255, 255), CRGB(0, 31, 255)})
#define BLUE_WHITISH_GRADIENT CRGBPalette(3, (CRGB[]){CRGB(0, 31, 255), CRGB(191, 191, 191), CRGB(0, 31, 255)})

#define BLUE_YELLOW_BLACK_GRADIENT CRGBPalette(5, (CRGB[]){CRGB(0, 95, 255), CRGB(255, 255, 0), CRGB::White, CRGB::Black, CRGB(0, 31, 255)})
#define RED_WHITE_BLACK_GRADIENT CRGBPalette(5, (CRGB[]){CRGB(255, 0, 31), CRGB(255, 0, 0), CRGB::White, CRGB::Black, CRGB(255, 0, 31)})

#define RED_WHITE_GRADIENT CRGBPalette(4, (CRGB[]){CRGB(255, 31, 0), CRGB(255, 0, 0), CRGB(255, 255, 255), CRGB(255, 31, 0)})
#define GREEN_WHITE_GRADIENT CRGBPalette(4, (CRGB[]){CRGB(0, 255, 31), CRGB(0, 255, 0), CRGB(255, 255, 255), CRGB(0, 255, 31)})
#define RED_WHITISH_GRADIENT CRGBPalette(4, (CRGB[]){CRGB(255, 31, 0), CRGB(255, 0, 0), CRGB(191, 191, 191), CRGB(255, 31, 0)})

// Palettes

/*
PALLETE_5(rainbowBlackGradient, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black, CRGB::Red);

PALLETE_2_M(pinkPurpleGradient, CRGB(255, 0, 128), CRGB(96, 0, 192));
PALLETE_2_M(solidRedGradient, CRGB(255, 192, 0), CRGB(255, 192, 0));

PALLETE_5(pinkPurpleBlackGradient, CRGB(255, 0, 128), CRGB(96, 0, 192), CRGB::Black, CRGB::Black, CRGB(255, 0, 128));

PALLETE_2_M(cyanPinkGradient, CRGB(0, 255, 255), CRGB(255, 0, 96));

PALLETE_5(cyanPinkBlackGradient, CRGB(0, 255, 192), CRGB::Black, CRGB(255, 16, 64), CRGB::Black, CRGB(0, 255, 192));

PALLETE_5(greenBlueBlackGradient, CRGB(0, 255, 64), CRGB(0, 0, 255), CRGB::Black, CRGB::Black, CRGB(0, 255, 64));

PALLETE_5(redOrgangeBlackGradient, CRGB(255, 31, 0), CRGB(255, 191, 0), CRGB::Black, CRGB::Black, CRGB(255, 31, 0));

PALLETE_5(blueWhiteBlackGradient, CRGB(0, 31, 25), CRGB(255, 255, 255), CRGB::White, CRGB::Black, CRGB(0, 31, 255));


PALLETE_4(blueYellowBlackHardGradient, CRGB(0, 0, 0), CRGB(16, 16, 0), CRGB(255, 255, 0), CRGB(0, 95, 255));

PALLETE_3(blueYellowGradient, CRGB(0, 95, 255), CRGB(255, 255, 0), CRGB(0, 31, 255));
PALLETE_3(greenYellowGradient, CRGB(31, 255, 31), CRGB(255, 191, 63), CRGB(31, 255, 31));

PALLETE_2(whiteSolidGradient, CRGB(64, 64, 64), CRGB(64, 64, 64));
*/


#define CHANNEL_SIZE 3
#define CUSTOM_PALETTE_SIZE (1 + (CHANNEL_SIZE * 8))
#define EEPROM_OFFSET 32

class PaletteManager {
protected:
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
	// void randomizeOrder();

	uint8_t paletteCount();

	void setCustomPalettes(uint8_t customPalettes, bool reset = false);
	
	CRGB getColor(uint8_t index);

	void add(CRGBPalette palette);
	void setActive(CRGBPalette &palette);
	CRGBPalette &getActive() { return currentPalette; }

	void loadPalette(uint8_t index);
	
	void next();
	void previous();
};

extern PaletteManager Palettes;

#endif