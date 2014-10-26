#include "LSCPGradientPalette.h"
#include <avr/pgmspace.h>

LSColorPalette *factoryGradientPalette() {
	return new LSCPGradientPalette();
}

LSCPGradientPalette::LSCPGradientPalette() 
	: LSColorPalette()
{
	colors[0] = CRGB::Red;
	colors[1] = CRGB::Green;
	colors[2] = CRGB::Blue;
	
	sections = 2;
	mirrored = false;	
}

void LSCPGradientPalette::setConfig(void *_config) {
	pgradient_palette_func func = (pgradient_palette_func)_config;
	gradient_palette_t config = func();

	memcpy(this->colors, config.colors, sizeof(CRGB) * MAX_GRADIENT_COLORS);
	
	this->sections = config.sections;
	this->mirrored = config.mirrored;
}

CRGB LSCPGradientPalette::getColor(uint8_t index) {
	uint16_t totalColors = 0x100;
	
	if (mirrored == true) {
		if (index > (totalColors >> 1))
			return getColor(totalColors - index);
		totalColors >>= 1;
	}

	uint16_t indexSections = index * sections;
	uint8_t section = indexSections / totalColors;
	
	uint8_t weight = ((indexSections << 8) / totalColors) - (section << 8);

	CRGB col = colors[section];
	col.lerp8(colors[section + 1], weight);

	return col;
}

LSColorPalette *factoryTabledGradientPalette() {
	return new LSCPTabledGradientPalette();
}

bool LSCPTabledGradientPalette::usePaletteTable() {
	return true;
}

CRGB LSCPTabledGradientPalette::getColor(uint8_t index) {
	return paletteTable[index];
}

CRGB LSCPTabledGradientPalette::getTableColor(uint8_t index) {
	return LSCPGradientPalette::getColor(index);
}

// ------------------------ Palettes ------------------------

PALLETE_4(rainbowGradient, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Red);
PALLETE_5(rainbowBlackGradient, CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black, CRGB::Red);

PALLETE_2_M(pinkPurpleGradient, CRGB(255, 0, 128), CRGB(96, 0, 192));
PALLETE_2_M(solidRedGradient, CRGB(255, 192, 0), CRGB(255, 192, 0));

PALLETE_5(pinkPurpleBlackGradient, CRGB(255, 0, 128), CRGB(96, 0, 192), CRGB::Black, CRGB::Black, CRGB(255, 0, 128));

PALLETE_2_M(cyanPinkGradient, CRGB(0, 255, 255), CRGB(255, 0, 96));

PALLETE_5(cyanPinkBlackGradient, CRGB(0, 255, 192), CRGB::Black, CRGB(255, 16, 64), CRGB::Black, CRGB(0, 255, 192));

PALLETE_2_M(blueGreenGradient, CRGB::Blue, CRGB(0, 255, 64));
PALLETE_2_M(greenGradient, CRGB::Green, CRGB(0, 32, 0));
PALLETE_2_M(yellowGradient, CRGB(255, 191, 63), CRGB(63, 47, 15));
PALLETE_2_M(whiteGradient, CRGB::White, CRGB::Black);

PALLETE_5(greenBlueBlackGradient, CRGB(0, 255, 64), CRGB(0, 0, 255), CRGB::Black, CRGB::Black, CRGB(0, 255, 64));

PALLETE_3(greenBlueGradient, CRGB(0, 255, 64), CRGB(0, 0, 255), CRGB(0, 255, 64));
PALLETE_3_M(redOrgangeGradient, CRGB(128, 128, 128), CRGB(128, 31, 0), CRGB(128, 95, 0));

PALLETE_5(redOrgangeBlackGradient, CRGB(255, 31, 0), CRGB(255, 191, 0), CRGB::Black, CRGB::Black, CRGB(255, 31, 0));
PALLETE_5(redWhiteBlackGradient, CRGB(255, 0, 31), CRGB(255, 0, 0), CRGB::White, CRGB::Black, CRGB(255, 0, 31));

PALLETE_4(redWhiteGradient, CRGB(255, 31, 0), CRGB(255, 0, 0), CRGB(255, 255, 255), CRGB(255, 31, 0));
PALLETE_4(greenWhiteGradient, CRGB(0, 255, 31), CRGB(0, 255, 0), CRGB(255, 255, 255), CRGB(0, 255, 31));
PALLETE_4(redWhitishGradient, CRGB(255, 31, 0), CRGB(255, 0, 0), CRGB(191, 191, 191), CRGB(255, 31, 0));

PALLETE_5(blueWhiteBlackGradient, CRGB(0, 31, 25), CRGB(255, 255, 255), CRGB::White, CRGB::Black, CRGB(0, 31, 255));

PALLETE_3(blueWhiteGradient, CRGB(0, 31, 255), CRGB(255, 255, 255), CRGB(0, 31, 255));
PALLETE_3(blueWhitishGradient, CRGB(0, 31, 255), CRGB(191, 191, 191), CRGB(0, 31, 255));

PALLETE_5(blueYellowBlackGradient, CRGB(0, 95, 255), CRGB(255, 255, 0), CRGB::White, CRGB::Black, CRGB(0, 31, 255));

PALLETE_4(blueYellowBlackHardGradient, CRGB(0, 0, 0), CRGB(16, 16, 0), CRGB(255, 255, 0), CRGB(0, 95, 255));

PALLETE_3(blueYellowGradient, CRGB(0, 95, 255), CRGB(255, 255, 0), CRGB(0, 31, 255));
PALLETE_3(greenYellowGradient, CRGB(31, 255, 31), CRGB(255, 191, 63), CRGB(31, 255, 31));

PALLETE_2(whiteSolidGradient, CRGB(64, 64, 64), CRGB(64, 64, 64));
