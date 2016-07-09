#include "palette.h"

using namespace LightString;

SwatchManager<10, RGBAu> Swatches;
PaletteManager<20, RGBAu> Palettes;

namespace LightString {
	TPalette<RGBAu> PROGMEM SOLID_RED 										= TPalette<RGBAu>(HTML::Red, HTML::Red);
	TPalette<RGBAu> PROGMEM SOLID_GREEN									= TPalette<RGBAu>(HTML::Lime, HTML::Lime);
	TPalette<RGBAu> PROGMEM SOLID_BLUE										= TPalette<RGBAu>(HTML::Blue, HTML::Blue);
	TPalette<RGBAu> PROGMEM SOLID_AQUA										= TPalette<RGBAu>(HTML::Aqua, HTML::Aqua);
	TPalette<RGBAu> PROGMEM SOLID_AZURE									= TPalette<RGBAu>(HTML::Azure, HTML::Azure);

	TPalette<RGBAu> PROGMEM RAINBOW_GRADIENT 						= TPalette<RGBAu>(HTML::Red, HTML::Lime, HTML::Blue, HTML::Red);

	TPalette<RGBAu> PROGMEM BLUE_GREEN_GRADIENT 					= TPalette<RGBAu>(HTML::Blue, RGBAu(0, 255, 64), HTML::Blue);
	TPalette<RGBAu> PROGMEM GREEN_GRADIENT 							= TPalette<RGBAu>(HTML::Lime, RGBAu(0, 32, 0), HTML::Lime);
	TPalette<RGBAu> PROGMEM LIME_GRADIENT 								= TPalette<RGBAu>(HTML::Lime, HTML::Green, HTML::Lime);
	TPalette<RGBAu> PROGMEM YELLOW_GRADIENT 							= TPalette<RGBAu>(RGBAu(255, 191, 63), RGBAu(63, 47, 15), RGBAu(255, 191, 63));
	TPalette<RGBAu> PROGMEM WHITE_GRADIENT 							= TPalette<RGBAu>(HTML::White, HTML::Black, HTML::White);

	TPalette<RGBAu> PROGMEM RED_GREEN_GRADIENT 					= TPalette<RGBAu>(HTML::Red, HTML::Green);

	TPalette<RGBAu> PROGMEM GREEN_BLUE_GRADIENT 					= TPalette<RGBAu>(RGBAu(0, 255, 64), RGBAu(0, 0, 255), RGBAu(0, 255, 64));
	TPalette<RGBAu> PROGMEM RED_ORGANGE_GRADIENT 				= TPalette<RGBAu>(RGBAu(128, 128, 128), RGBAu(128, 31, 0), RGBAu(128, 95, 0), RGBAu(128, 31, 0), RGBAu(128, 128, 128));

	TPalette<RGBAu> PROGMEM BLUE_WHITE_GRADIENT 					= TPalette<RGBAu>(RGBAu(0, 31, 255), RGBAu(255, 255, 255), RGBAu(0, 31, 255));
	TPalette<RGBAu> PROGMEM BLUE_WHITISH_GRADIENT				= TPalette<RGBAu>(RGBAu(0, 31, 255), RGBAu(191, 191, 191), RGBAu(0, 31, 255));

	TPalette<RGBAu> PROGMEM BLUE_WHITE_YELLOW_GRADIENT 	= TPalette<RGBAu>(RGBAu(0, 95, 255), RGBAu(255, 255, 0), HTML::White, RGBAu(0, 31, 255));

	TPalette<RGBAu> PROGMEM BLUE_YELLOW_BLACK_GRADIENT 	= TPalette<RGBAu>(RGBAu(0, 95, 255), RGBAu(255, 255, 0), HTML::White, HTML::Black, RGBAu(0, 31, 255));
	TPalette<RGBAu> PROGMEM RED_WHITE_BLACK_GRADIENT 		= TPalette<RGBAu>(RGBAu(255, 0, 31), RGBAu(255, 0, 0), HTML::White, HTML::Black, RGBAu(255, 0, 31));

	TPalette<RGBAu> PROGMEM RED_WHITE_GRADIENT 					= TPalette<RGBAu>(RGBAu(255, 31, 0), RGBAu(255, 0, 0), RGBAu(255, 255, 255), RGBAu(255, 31, 0));
	TPalette<RGBAu> PROGMEM GREEN_WHITE_GRADIENT 				= TPalette<RGBAu>(RGBAu(0, 255, 31), RGBAu(0, 255, 0), RGBAu(255, 255, 255), RGBAu(0, 255, 31));
	TPalette<RGBAu> PROGMEM RED_WHITISH_GRADIENT 				= TPalette<RGBAu>(RGBAu(255, 31, 0), RGBAu(255, 0, 0), RGBAu(191, 191, 191), RGBAu(255, 31, 0));

	TPalette<RGBAu> PROGMEM CYAN_PINK_GRADIENT 					= TPalette<RGBAu>(RGBAu(0, 255, 255), RGBAu(255, 0, 96), RGBAu(0, 255, 255));
	TPalette<RGBAu> PROGMEM BLUE_YELLOW_GRADIENT 				= TPalette<RGBAu>(RGBAu(0, 95, 255), RGBAu(255, 255, 0), RGBAu(0, 31, 255));
	TPalette<RGBAu> PROGMEM GREEN_YELLOW_GRADIENT 				= TPalette<RGBAu>(RGBAu(31, 255, 31), RGBAu(255, 191, 63), RGBAu(31, 255, 31));
	TPalette<RGBAu> PROGMEM RAINBOW_BLACK_GRADIENT 			= TPalette<RGBAu>(HTML::Red, HTML::Green, HTML::Blue, HTML::Black, HTML::Red);

};

void printColor(RGBAu col) {
	printf("(%d, %d, %d, %d)", col.r, col.g, col.b, col.a);
}

