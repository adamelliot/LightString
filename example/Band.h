#ifndef _SPIN_H_
#define _SPIN_H_

#include <FastLED.h>
#include <LightString.h>

class Band : public LightProgram {
	uint8_t colorIndex;
	uint8_t indexStep;
	uint8_t fadeRate;
	
	CRGB color;

	float start, end;
	float deltaS, deltaE;
	
public:
	Band(CRGB color = CRGB::Pink)
		: LightProgram(1), color(color) {}

	void setupMode(uint8_t mode);
	void update(uint32_t ms);
};

#endif
