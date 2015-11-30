#ifndef _WAVE_H_
#define _WAVE_H_

#include <FastLED.h>
#include <LightString.h>

class Wave : public FilterLightProgram {
	uint8_t shift;
	uint8_t sections;

public:

	Wave() : FilterLightProgram(1) {}

	void setupMode(uint8_t mode) {
		sections = random8(4, 8);
	}

	void update(uint32_t ms) {
		shift++;

		uint16_t len = pixelBuffer->length / sections;

		for (uint16_t i = 0; i < pixelBuffer->length; i++) {
			uint8_t x = (i + shift) * 255 / len;
			pixelBuffer->pixels[i].nscale8(sin8(x));
		}
	}
	
};

#endif