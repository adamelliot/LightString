#include "palette.h"

using namespace LightString;

TSwatchManager<TRGBA, uint8_t> Swatches(20);
TPaletteManager<TRGBA, uint8_t> Palettes(10);

namespace LightString {

const ColorPaletteData PROGMEM SOLID_RED 						= { HTML::Red };
const ColorPaletteData PROGMEM SOLID_GREEN						= { HTML::Lime };
const ColorPaletteData PROGMEM SOLID_BLUE						= { HTML::Blue };
const ColorPaletteData PROGMEM SOLID_AQUA						= { HTML::Aqua };
const ColorPaletteData PROGMEM SOLID_AZURE						= { HTML::Azure };
const ColorPaletteData PROGMEM HSV_GRADIENT						= { HTML::Red, 0xffff00, HTML::Lime, 0x00ffff, HTML::Blue, 0xff00ff, HTML::Red };
const ColorPaletteData PROGMEM RAINBOW_GRADIENT 				= { HTML::Red, HTML::Lime, HTML::Blue, HTML::Red };
const ColorPaletteData PROGMEM BLUE_GREEN_GRADIENT 				= { HTML::Blue, RGB_HEX(0, 255, 64), HTML::Blue };
const ColorPaletteData PROGMEM GREEN_GRADIENT 					= { HTML::Lime, RGB_HEX(0, 32, 0), HTML::Lime };
const ColorPaletteData PROGMEM LIME_GRADIENT 					= { HTML::Lime, HTML::Green, HTML::Lime };
const ColorPaletteData PROGMEM YELLOW_GRADIENT 					= { RGB_HEX(255, 191, 63), RGB_HEX(63, 47, 15), RGB_HEX(255, 191, 63) };
const ColorPaletteData PROGMEM WHITE_GRADIENT 					= { HTML::White, HTML::Black, HTML::White };
const ColorPaletteData PROGMEM RED_GREEN_GRADIENT 				= { HTML::Red, HTML::Green };
const ColorPaletteData PROGMEM GREEN_BLUE_GRADIENT 				= { RGB_HEX(0, 255, 64), RGB_HEX(0, 0, 255), RGB_HEX(0, 255, 64) };
const ColorPaletteData PROGMEM RED_ORGANGE_GRADIENT 			= { RGB_HEX(128, 128, 128), RGB_HEX(128, 31, 0), RGB_HEX(128, 95, 0), RGB_HEX(128, 31, 0), RGB_HEX(128, 128, 128) };
const ColorPaletteData PROGMEM BLUE_WHITE_GRADIENT 				= { RGB_HEX(0, 31, 255), RGB_HEX(255, 255, 255), RGB_HEX(0, 31, 255) };
const ColorPaletteData PROGMEM BLUE_WHITISH_GRADIENT			= { RGB_HEX(0, 31, 255), RGB_HEX(191, 191, 191), RGB_HEX(0, 31, 255) };
const ColorPaletteData PROGMEM BLUE_WHITE_YELLOW_GRADIENT 		= { RGB_HEX(0, 95, 255), RGB_HEX(255, 255, 0), HTML::White, RGB_HEX(0, 31, 255) };
const ColorPaletteData PROGMEM BLUE_YELLOW_BLACK_GRADIENT 		= { RGB_HEX(0, 95, 255), RGB_HEX(255, 255, 0), HTML::White, HTML::Black, RGB_HEX(0, 31, 255) };
const ColorPaletteData PROGMEM RED_WHITE_BLACK_GRADIENT 		= { RGB_HEX(255, 0, 31), RGB_HEX(255, 0, 0), HTML::White, HTML::Black, RGB_HEX(255, 0, 31) };
const ColorPaletteData PROGMEM RED_WHITE_GRADIENT 				= { RGB_HEX(255, 31, 0), RGB_HEX(255, 0, 0), RGB_HEX(255, 255, 255), RGB_HEX(255, 31, 0) };
const ColorPaletteData PROGMEM GREEN_WHITE_GRADIENT 			= { RGB_HEX(0, 255, 31), RGB_HEX(0, 255, 0), RGB_HEX(255, 255, 255), RGB_HEX(0, 255, 31) };
const ColorPaletteData PROGMEM RED_WHITISH_GRADIENT 			= { RGB_HEX(255, 31, 0), RGB_HEX(255, 0, 0), RGB_HEX(191, 191, 191), RGB_HEX(255, 31, 0) };
const ColorPaletteData PROGMEM CYAN_PINK_GRADIENT 				= { RGB_HEX(0, 255, 255), RGB_HEX(255, 0, 96), RGB_HEX(0, 255, 255) };
const ColorPaletteData PROGMEM BLUE_YELLOW_GRADIENT 			= { RGB_HEX(0, 95, 255), RGB_HEX(255, 255, 0), RGB_HEX(0, 31, 255) };
const ColorPaletteData PROGMEM GREEN_YELLOW_GRADIENT 			= { RGB_HEX(31, 255, 31), RGB_HEX(255, 191, 63), RGB_HEX(31, 255, 31) };
const ColorPaletteData PROGMEM RAINBOW_BLACK_GRADIENT 			= { HTML::Red, HTML::Green, HTML::Blue, HTML::Black, HTML::Red };
const ColorPaletteData PROGMEM PINK_PURPLE_GRADIENT				= { RGB_HEX(255, 0, 128), RGB_HEX(96, 0, 192), RGB_HEX(255, 0, 128) };
const ColorPaletteData PROGMEM PINK_PURPLE_BLACK_GRADIENT		= { RGB_HEX(255, 0, 128), RGB_HEX(96, 0, 192), HTML::Black, HTML::Black, RGB_HEX(255, 0, 128) };
const ColorPaletteData PROGMEM CYAN_PINK_BLACK_GRADIENT			= { RGB_HEX(0, 255, 192), HTML::Black, RGB_HEX(255, 16, 64), HTML::Black, RGB_HEX(0, 255, 192) };
const ColorPaletteData PROGMEM GREEN_BLUE_BLACK_GRADIENT		= { RGB_HEX(0, 255, 64), RGB_HEX(0, 0, 255), HTML::Black, HTML::Black, RGB_HEX(0, 255, 64) };
const ColorPaletteData PROGMEM RED_ORGANGE_BLACK_GRADIENT		= { RGB_HEX(255, 31, 0), RGB_HEX(255, 191, 0), HTML::Black, HTML::Black, RGB_HEX(255, 31, 0) };
const ColorPaletteData PROGMEM BLUE_WHITE_BLACK_GRADIENT		= { RGB_HEX(0, 31, 25), RGB_HEX(255, 255, 255), HTML::White, HTML::Black, RGB_HEX(0, 31, 255) };
const ColorPaletteData PROGMEM BLUE_YELLOW_BLACK_HARD_GRADIENT	= { RGB_HEX(0, 0, 0), RGB_HEX(16, 16, 0), RGB_HEX(255, 255, 0), RGB_HEX(0, 95, 255) };
const ColorPaletteData PROGMEM WHITE_SOLID_GRADIENT				= { RGB_HEX(64, 64, 64), RGB_HEX(64, 64, 64) };


};
