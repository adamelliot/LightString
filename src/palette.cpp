#include "palette.h"

using namespace LightString;

SwatchManager<10, RGBAu> Swatches;
PaletteManager<20, RGBAu> Palettes;

namespace LightString {
	// TPalette<RGBAu> PROGMEM SOLID_RED 										= TPalette<RGBAu>(CRGB::Red, CRGB::Red);
	// TPalette<RGBAu> PROGMEM SOLID_GREEN									= TPalette<RGBAu>(CRGB::Lime, CRGB::Lime);
	// TPalette<RGBAu> PROGMEM SOLID_BLUE										= TPalette<RGBAu>(CRGB::Blue, CRGB::Blue);
	// TPalette<RGBAu> PROGMEM SOLID_AQUA										= TPalette<RGBAu>(CRGB::Aqua, CRGB::Aqua);
	// TPalette<RGBAu> PROGMEM SOLID_AZURE									= TPalette<RGBAu>(CRGB::Azure, CRGB::Azure);

	// TPalette<RGBAu> PROGMEM RAINBOW_GRADIENT 						= TPalette<RGBAu>(CRGB::Red, CRGB::Lime, CRGB::Blue, CRGB::Red);

	// TPalette<RGBAu> PROGMEM BLUE_GREEN_GRADIENT 					= TPalette<RGBAu>(CRGB::Blue, CRGB(0, 255, 64), CRGB::Blue);
	// TPalette<RGBAu> PROGMEM GREEN_GRADIENT 							= TPalette<RGBAu>(CRGB::Lime, CRGB(0, 32, 0), CRGB::Lime);
	// TPalette<RGBAu> PROGMEM LIME_GRADIENT 								= TPalette<RGBAu>(CRGB::Lime, CRGB::Green, CRGB::Lime);
	// TPalette<RGBAu> PROGMEM YELLOW_GRADIENT 							= TPalette<RGBAu>(CRGB(255, 191, 63), CRGB(63, 47, 15), CRGB(255, 191, 63));
	// TPalette<RGBAu> PROGMEM WHITE_GRADIENT 							= TPalette<RGBAu>(CRGB::White, CRGB::Black, CRGB::White);

	// TPalette<RGBAu> PROGMEM RED_GREEN_GRADIENT 					= TPalette<RGBAu>(CRGB::Red, CRGB::Green);

	TPalette<RGBAu> PROGMEM GREEN_BLUE_GRADIENT 					= TPalette<RGBAu>(RGBAu(0, 255, 64), RGBAu(0, 0, 255), RGBAu(0, 255, 64));
	TPalette<RGBAu> PROGMEM RED_ORGANGE_GRADIENT 				= TPalette<RGBAu>(RGBAu(128, 128, 128), RGBAu(128, 31, 0), RGBAu(128, 95, 0), RGBAu(128, 31, 0), RGBAu(128, 128, 128));

	TPalette<RGBAu> PROGMEM BLUE_WHITE_GRADIENT 					= TPalette<RGBAu>(RGBAu(0, 31, 255), RGBAu(255, 255, 255), RGBAu(0, 31, 255));
	TPalette<RGBAu> PROGMEM BLUE_WHITISH_GRADIENT				= TPalette<RGBAu>(RGBAu(0, 31, 255), RGBAu(191, 191, 191), RGBAu(0, 31, 255));

	// TPalette<RGBAu> PROGMEM BLUE_WHITE_YELLOW_GRADIENT 	= TPalette<RGBAu>(CRGB(0, 95, 255), CRGB(255, 255, 0), CRGB::White, CRGB(0, 31, 255));

	// TPalette<RGBAu> PROGMEM BLUE_YELLOW_BLACK_GRADIENT 	= TPalette<RGBAu>(CRGB(0, 95, 255), CRGB(255, 255, 0), CRGB::White, CRGB::Black, CRGB(0, 31, 255));
	// TPalette<RGBAu> PROGMEM RED_WHITE_BLACK_GRADIENT 		= TPalette<RGBAu>(CRGB(255, 0, 31), CRGB(255, 0, 0), CRGB::White, CRGB::Black, CRGB(255, 0, 31));

	TPalette<RGBAu> PROGMEM RED_WHITE_GRADIENT 					= TPalette<RGBAu>(RGBAu(255, 31, 0), RGBAu(255, 0, 0), RGBAu(255, 255, 255), RGBAu(255, 31, 0));
	TPalette<RGBAu> PROGMEM GREEN_WHITE_GRADIENT 				= TPalette<RGBAu>(RGBAu(0, 255, 31), RGBAu(0, 255, 0), RGBAu(255, 255, 255), RGBAu(0, 255, 31));
	TPalette<RGBAu> PROGMEM RED_WHITISH_GRADIENT 				= TPalette<RGBAu>(RGBAu(255, 31, 0), RGBAu(255, 0, 0), RGBAu(191, 191, 191), RGBAu(255, 31, 0));

	TPalette<RGBAu> PROGMEM CYAN_PINK_GRADIENT 					= TPalette<RGBAu>(RGBAu(0, 255, 255), RGBAu(255, 0, 96), RGBAu(0, 255, 255));
	TPalette<RGBAu> PROGMEM BLUE_YELLOW_GRADIENT 				= TPalette<RGBAu>(RGBAu(0, 95, 255), RGBAu(255, 255, 0), RGBAu(0, 31, 255));
	TPalette<RGBAu> PROGMEM GREEN_YELLOW_GRADIENT 				= TPalette<RGBAu>(RGBAu(31, 255, 31), RGBAu(255, 191, 63), RGBAu(31, 255, 31));
	// TPalette<RGBAu> PROGMEM RAINBOW_BLACK_GRADIENT 			= TPalette<RGBAu>(CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black, CRGB::Red);

};

void printColor(RGBAu col) {
	printf("(%d, %d, %d, %d)", col.r, col.g, col.b, col.a);
}

