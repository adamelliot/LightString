#include "palette.h"

using namespace LightString;

SwatchManager<10, LightString::RGBA> Swatches;
PaletteManager<20, LightString::RGBA> Palettes;

namespace LightString {

	TPalette<RGBA> PROGMEM SOLID_RED 										= TPalette<RGBA>(CRGB::Red, CRGB::Red);
	TPalette<RGBA> PROGMEM SOLID_GREEN									= TPalette<RGBA>(CRGB::Lime, CRGB::Lime);
	TPalette<RGBA> PROGMEM SOLID_BLUE									= TPalette<RGBA>(CRGB::Blue, CRGB::Blue);
	TPalette<RGBA> PROGMEM SOLID_AQUA									= TPalette<RGBA>(CRGB::Aqua, CRGB::Aqua);
	TPalette<RGBA> PROGMEM SOLID_AZURE									= TPalette<RGBA>(CRGB::Azure, CRGB::Azure);
  
	TPalette<RGBA> PROGMEM RAINBOW_GRADIENT 						= TPalette<RGBA>(CRGB::Red, CRGB::Lime, CRGB::Blue, CRGB::Red);
  
	TPalette<RGBA> PROGMEM BLUE_GREEN_GRADIENT 				= TPalette<RGBA>(CRGB::Blue, CRGB(0, 255, 64), CRGB::Blue);
	TPalette<RGBA> PROGMEM GREEN_GRADIENT 							= TPalette<RGBA>(CRGB::Lime, CRGB(0, 32, 0), CRGB::Lime);
	TPalette<RGBA> PROGMEM LIME_GRADIENT 							= TPalette<RGBA>(CRGB::Lime, CRGB::Green, CRGB::Lime);
	TPalette<RGBA> PROGMEM YELLOW_GRADIENT 						= TPalette<RGBA>(CRGB(255, 191, 63), CRGB(63, 47, 15), CRGB(255, 191, 63));
	TPalette<RGBA> PROGMEM WHITE_GRADIENT 							= TPalette<RGBA>(CRGB::White, CRGB::Black, CRGB::White);
  
	TPalette<RGBA> PROGMEM RED_GREEN_GRADIENT 					= TPalette<RGBA>(CRGB::Red, CRGB::Green);
  
	TPalette<RGBA> PROGMEM GREEN_BLUE_GRADIENT 				= TPalette<RGBA>(CRGB(0, 255, 64), CRGB(0, 0, 255), CRGB(0, 255, 64));
	TPalette<RGBA> PROGMEM RED_ORGANGE_GRADIENT 				= TPalette<RGBA>(CRGB(128, 128, 128), CRGB(128, 31, 0), CRGB(128, 95, 0), CRGB(128, 31, 0), CRGB(128, 128, 128));
  
	TPalette<RGBA> PROGMEM BLUE_WHITE_GRADIENT 				= TPalette<RGBA>(CRGB(0, 31, 255), CRGB(255, 255, 255), CRGB(0, 31, 255));
	TPalette<RGBA> PROGMEM BLUE_WHITISH_GRADIENT				= TPalette<RGBA>(CRGB(0, 31, 255), CRGB(191, 191, 191), CRGB(0, 31, 255));
  
	TPalette<RGBA> PROGMEM BLUE_WHITE_YELLOW_GRADIENT 	= TPalette<RGBA>(CRGB(0, 95, 255), CRGB(255, 255, 0), CRGB::White, CRGB(0, 31, 255));
  
	TPalette<RGBA> PROGMEM BLUE_YELLOW_BLACK_GRADIENT 	= TPalette<RGBA>(CRGB(0, 95, 255), CRGB(255, 255, 0), CRGB::White, CRGB::Black, CRGB(0, 31, 255));
	TPalette<RGBA> PROGMEM RED_WHITE_BLACK_GRADIENT 		= TPalette<RGBA>(CRGB(255, 0, 31), CRGB(255, 0, 0), CRGB::White, CRGB::Black, CRGB(255, 0, 31));
  
	TPalette<RGBA> PROGMEM RED_WHITE_GRADIENT 					= TPalette<RGBA>(CRGB(255, 31, 0), CRGB(255, 0, 0), CRGB(255, 255, 255), CRGB(255, 31, 0));
	TPalette<RGBA> PROGMEM GREEN_WHITE_GRADIENT 				= TPalette<RGBA>(CRGB(0, 255, 31), CRGB(0, 255, 0), CRGB(255, 255, 255), CRGB(0, 255, 31));
	TPalette<RGBA> PROGMEM RED_WHITISH_GRADIENT 				= TPalette<RGBA>(CRGB(255, 31, 0), CRGB(255, 0, 0), CRGB(191, 191, 191), CRGB(255, 31, 0));
  
	TPalette<RGBA> PROGMEM CYAN_PINK_GRADIENT 					= TPalette<RGBA>(CRGB(0, 255, 255), CRGB(255, 0, 96), CRGB(0, 255, 255));
	TPalette<RGBA> PROGMEM BLUE_YELLOW_GRADIENT 				= TPalette<RGBA>(CRGB(0, 95, 255), CRGB(255, 255, 0), CRGB(0, 31, 255));
	TPalette<RGBA> PROGMEM GREEN_YELLOW_GRADIENT 			= TPalette<RGBA>(CRGB(31, 255, 31), CRGB(255, 191, 63), CRGB(31, 255, 31));
	TPalette<RGBA> PROGMEM RAINBOW_BLACK_GRADIENT 			= TPalette<RGBA>(CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Black, CRGB::Red);

};

void printColor(LightString::RGBA col) {
	Serial.print("(");
	Serial.print(col.r);
	Serial.print(",");
	Serial.print(col.g);
	Serial.print(",");
	Serial.print(col.b);
	Serial.print(",");
	Serial.print(col.a);
	Serial.print(")");
}

