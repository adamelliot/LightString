#ifndef _LSCPSOLID_PALETTE_H_
#define _LSCPSOLID_PALETTE_H_

#include "LSLightString.h"

class LSCPSolidPalette : public LSColorPalette {
private:
	CRGB color;

public:
	
	void setConfig(void *_config);
	CRGB getColor(uint8_t index);
};

LSColorPalette *factorySolidPalette();

#endif