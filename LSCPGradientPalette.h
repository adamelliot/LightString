#ifndef _LSCPGRADIENT_PALETTE_H_
#define _LSCPGRADIENT_PALETTE_H_

#include "LSLightString.h"

#define MAX_GRADIENT_COLORS 5

typedef struct gradient_palette_s gradient_palette_t, *pgradient_palette_t;
typedef gradient_palette_t (*pgradient_palette_func)(void);

gradient_palette_t rainbowGradient();
gradient_palette_t rainbowBlackGradient();
//gradient_palette_t rainbowBlackBlackGradient();
gradient_palette_t pinkPurpleGradient();
gradient_palette_t solidRedGradient();
gradient_palette_t pinkPurpleBlackGradient();
gradient_palette_t cyanPinkGradient();
gradient_palette_t cyanPinkBlackGradient();
gradient_palette_t blueGreenGradient();
gradient_palette_t yellowGradient();
gradient_palette_t whiteGradient();
gradient_palette_t greenGradient();
gradient_palette_t greenBlueGradient();
gradient_palette_t greenBlueBlackGradient();
gradient_palette_t redOrgangeGradient();
gradient_palette_t redOrgangeBlackGradient();
gradient_palette_t redWhiteGradient();
gradient_palette_t greenWhiteGradient();
gradient_palette_t redWhitishGradient();
gradient_palette_t redWhiteBlackGradient();
gradient_palette_t blueWhiteGradient();
gradient_palette_t blueWhitishGradient();
gradient_palette_t blueWhiteBlackGradient();
gradient_palette_t blueYellowGradient();
gradient_palette_t blueYellowBlackGradient();
gradient_palette_t blueYellowBlackHardGradient();
gradient_palette_t greenYellowGradient();
gradient_palette_t whiteSolidGradient();

struct gradient_palette_s {
	color_t colors[MAX_GRADIENT_COLORS];
	uint8_t sections;
	bool mirrored;
};

#define COL(R, G, B) ((color_t){R, G, B})

#define COL_BLACK 	COL(0, 0, 0)
#define COL_WHITE 	COL(255, 255, 255)
#define COL_RED 		COL(255, 0, 0)
#define COL_GREEN 	COL(0, 255, 0)
#define COL_BLUE 		COL(0, 0, 255)

#define PALLETE_2(NAME, A, B) gradient_palette_t NAME() { \
	return (gradient_palette_t){ {A, B}, 2, false }; }
#define PALLETE_3(NAME, A, B, C) gradient_palette_t NAME() { \
	return (gradient_palette_t){ {A, B, C}, 3, false }; }
#define PALLETE_4(NAME, A, B, C, D) gradient_palette_t NAME() { \
	return (gradient_palette_t){ {A, B, C, D}, 4, false }; }
#define PALLETE_5(NAME, A, B, C, D, E) gradient_palette_t NAME() { \
	return (gradient_palette_t){ {A, B, C, D, E}, 5, false }; }

#define PALLETE_2_M(NAME, A, B) gradient_palette_t NAME() { \
	return (gradient_palette_t){ {A, B}, 2, true }; }
#define PALLETE_3_M(NAME, A, B, C) gradient_palette_t NAME() { \
	return (gradient_palette_t){ {A, B, C}, 3, true }; }
#define PALLETE_4_M(NAME, A, B, C, D) gradient_palette_t NAME() { \
	return (gradient_palette_t){ {A, B, C, D}, 4, true }; }
#define PALLETE_5_M(NAME, A, B, C, D, E) gradient_palette_t NAME() { \
	return (gradient_palette_t){ {A, B, C, D, E}, 5, true }; }

class LSCPGradientPalette : public LSColorPalette {
private:
	color_t colors[MAX_GRADIENT_COLORS];
	uint8_t sections;
	bool mirrored;

	color_t interpolate(color_t col1, color_t col2, float weight);

public:
	LSCPGradientPalette();
	void setConfig(void *_config);
	virtual color_t getColor(uint8_t index);
};

LSColorPalette *factoryGradientPalette();

class LSCPTabledGradientPalette : public LSCPGradientPalette {
public:
	bool usePaletteTable();
	color_t getColor(uint8_t index);
	color_t getTableColor(uint8_t index);
};

LSColorPalette *factoryTabledGradientPalette();

#endif
