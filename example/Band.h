#ifndef _BAND_H_
#define _BAND_H_

#include <FastLED.h>
#include <LightString.h>

template <typename PIXEL>
class Band : public TLightProgram<PIXEL> {
	uint8_t colorIndex;
	uint8_t indexStep;
	uint8_t fadeRate;
	
	PIXEL color;

	float start, end;
	float deltaS, deltaE;
	
public:
	Band(PIXEL color = CRGB::Pink)
		: TLightProgram<PIXEL>(1), color(color) {}

	void setupMode(uint8_t mode);
	void update(uint32_t ms);
};

template <typename PIXEL>
void Band<PIXEL>::setupMode(uint8_t mode) {
	switch (mode) {
		case 0:
		break;
	}

	colorIndex = 0;
	indexStep = random8(6) + 1;

	start = (float)random8();
	end = (float)random8();

	deltaS = ((float)random8() - 127) / 100;
	deltaE = ((float)random8() - 127) / 100;
}

template <typename PIXEL>
void Band<PIXEL>::update(uint32_t ms) {
	this->pixelBuffer->clear();
	
	start += deltaS;
	end += deltaE;

	start = fmod(start, this->pixelBuffer->length);
	end = fmod(end, this->pixelBuffer->length);
	
	if (start < 0) start += this->pixelBuffer->length;
	if (end < 0) end += this->pixelBuffer->length;

	uint16_t sx = (uint16_t)start;
	uint16_t ex = (uint16_t)end;

	if (sx > ex) SWAP(sx, ex);

	uint16_t len = min(ex - sx, 15);
	this->pixelBuffer->setPixels(sx, len, color);
}


#endif
