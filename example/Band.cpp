#include "Band.h"

void Band::setupMode(uint8_t mode) {
	switch (mode) {
		case 0:
		break;
	}

	colorIndex = 0;
	indexStep = random8(6) + 1;
	
	start = (float)random8();
	end = (float)random8();

	deltaS = ((float)random8() - 127) / 200;
	deltaE = ((float)random8() - 127) / 200;
}

void Band::update(uint32_t ms) {
	pixelBuffer->clear();
	
	start += deltaS;
	end += deltaE;

	start = fmod(start, pixelBuffer->length);
	end = fmod(end, pixelBuffer->length);
	
	if (start < 0) start += pixelBuffer->length;
	if (end < 0) end += pixelBuffer->length;

	uint16_t sx = (uint16_t)start;
	uint16_t ex = (uint16_t)end;

	if (sx > ex) SWAP(sx, ex);

	uint16_t len = min(ex - sx, 20);
	pixelBuffer->setPixels(sx, len, color);

	LightProgram::update(ms);
}
