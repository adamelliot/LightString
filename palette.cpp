#include "palette.h"

using namespace LightString;

PaletteManager<20, CRGB> Palettes;

namespace LightString {

	CRGBPalette SOLID_RED										= CRGBPalette(CRGB::Red, CRGB::Red);
	CRGBPalette SOLID_GREEN									= CRGBPalette(CRGB::Lime, CRGB::Lime);
	CRGBPalette SOLID_BLUE									= CRGBPalette(CRGB::Blue, CRGB::Blue);
	CRGBPalette SOLID_AQUA									= CRGBPalette(CRGB::Aqua, CRGB::Aqua);
	CRGBPalette SOLID_AZURE									= CRGBPalette(CRGB::Azure, CRGB::Azure);

	CRGBPalette RAINBOW_GRADIENT 						= CRGBPalette(CRGB::Red, CRGB::Lime, CRGB::Blue, CRGB::Red);

	CRGBPalette BLUE_GREEN_GRADIENT 				= CRGBPalette(CRGB::Blue, CRGB(0, 255, 64), CRGB::Blue);
	CRGBPalette GREEN_GRADIENT 							= CRGBPalette(CRGB::Lime, CRGB(0, 32, 0), CRGB::Lime);
	CRGBPalette LIME_GRADIENT 							= CRGBPalette(CRGB::Lime, CRGB::Green, CRGB::Lime);
	CRGBPalette YELLOW_GRADIENT 						= CRGBPalette(CRGB(255, 191, 63), CRGB(63, 47, 15), CRGB(255, 191, 63));
	CRGBPalette WHITE_GRADIENT 							= CRGBPalette(CRGB::White, CRGB::Black, CRGB::White);

	CRGBPalette RED_GREEN_GRADIENT 					= CRGBPalette(CRGB::Red, CRGB::Green);

	CRGBPalette GREEN_BLUE_GRADIENT 				= CRGBPalette(CRGB(0, 255, 64), CRGB(0, 0, 255), CRGB(0, 255, 64));
	CRGBPalette RED_ORGANGE_GRADIENT 				= CRGBPalette(CRGB(128, 128, 128), CRGB(128, 31, 0), CRGB(128, 95, 0), CRGB(128, 31, 0), CRGB(128, 128, 128));

	CRGBPalette BLUE_WHITE_GRADIENT 				= CRGBPalette(CRGB(0, 31, 255), CRGB(255, 255, 255), CRGB(0, 31, 255));
	CRGBPalette BLUE_WHITISH_GRADIENT				= CRGBPalette(CRGB(0, 31, 255), CRGB(191, 191, 191), CRGB(0, 31, 255));

	CRGBPalette BLUE_WHITE_YELLOW_GRADIENT 	= CRGBPalette(CRGB(0, 95, 255), CRGB(255, 255, 0), CRGB::White, CRGB(0, 31, 255));

	CRGBPalette BLUE_YELLOW_BLACK_GRADIENT 	= CRGBPalette(CRGB(0, 95, 255), CRGB(255, 255, 0), CRGB::White, CRGB::Black, CRGB(0, 31, 255));
	CRGBPalette RED_WHITE_BLACK_GRADIENT 		= CRGBPalette(CRGB(255, 0, 31), CRGB(255, 0, 0), CRGB::White, CRGB::Black, CRGB(255, 0, 31));

	CRGBPalette RED_WHITE_GRADIENT 					= CRGBPalette(CRGB(255, 31, 0), CRGB(255, 0, 0), CRGB(255, 255, 255), CRGB(255, 31, 0));
	CRGBPalette GREEN_WHITE_GRADIENT 				= CRGBPalette(CRGB(0, 255, 31), CRGB(0, 255, 0), CRGB(255, 255, 255), CRGB(0, 255, 31));
	CRGBPalette RED_WHITISH_GRADIENT 				= CRGBPalette(CRGB(255, 31, 0), CRGB(255, 0, 0), CRGB(191, 191, 191), CRGB(255, 31, 0));

	CRGBPalette CYAN_PINK_GRADIENT 					= CRGBPalette(CRGB(0, 255, 255), CRGB(255, 0, 96), CRGB(0, 255, 255));
	CRGBPalette BLUE_YELLOW_GRADIENT 				= CRGBPalette(CRGB(0, 95, 255), CRGB(255, 255, 0), CRGB(0, 31, 255));
	CRGBPalette GREEN_YELLOW_GRADIENT 			= CRGBPalette(CRGB(31, 255, 31), CRGB(255, 191, 63), CRGB(31, 255, 31));
	CRGBPalette RAINBOW_BLACK_GRADIENT 			= CRGBPalette(CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black, CRGB::Red);

};

void printColor(CRGB col) {
	Serial.print("(");
	Serial.print(col.r);
	Serial.print(",");
	Serial.print(col.g);
	Serial.print(",");
	Serial.print(col.b);
	Serial.print(")");
}

