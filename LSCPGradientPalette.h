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
gradient_palette_t greenBlueGradient();
gradient_palette_t greenBlueBlackGradient();
gradient_palette_t redOrgangeGradient();
gradient_palette_t redOrgangeBlackGradient();
gradient_palette_t redWhiteGradient();
gradient_palette_t redWhiteBlackGradient();
gradient_palette_t blueWhiteGradient();
gradient_palette_t blueWhiteBlackGradient();
gradient_palette_t blueYellowGradient();
gradient_palette_t blueYellowBlackGradient();
gradient_palette_t blueYellowBlackHardGradient();
gradient_palette_t whiteSolidGradient();

struct gradient_palette_s {
	color_t colors[MAX_GRADIENT_COLORS];
	uint8_t sections;
	bool mirrored;
};

class LSCPGradientPalette : public LSColorPalette {
private:
	color_t colors[MAX_GRADIENT_COLORS];
	uint8_t sections;
	bool mirrored;

	color_t interpolate(color_t col1, color_t col2, float weight);

public:
	LSCPGradientPalette();
	void setConfig(void *_config);
	color_t getColor(uint8_t index);
};

LSColorPalette *factoryGradientPalette();

#endif
