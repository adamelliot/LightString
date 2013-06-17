#ifndef _LSCPSOLID_PALETTE_H_
#define _LSCPSOLID_PALETTE_H_

#include "LSLightString.h"

class LSCPSolidPalette : public LSColorPalette {
private:
	color_t color;

public:
	
	void setConfig(void *_config);
	color_t getColor(uint8_t index);
};

LSColorPalette *factorySolidPalette();

#endif