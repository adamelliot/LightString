#pragma once

#include "LightString.h"

#define WHEEL_PATTERN 0x10

using namespace LightString;

class WheelPattern : public TLightPattern<TRGB, uint8_t> {
private:

	uint32_t counter;

public:
	// Pass your parent constructor the count of modes this pattern has
	WheelPattern() : TLightPattern(1) {}

	// Specify a unique ID to identify this pattern. This can be used to load this pattern
	// from other patterns.
	// If you don't specify an ID they will all just be pooled under the general 0 id
	uint8_t getPatternID() { return WHEEL_PATTERN; }

	// Called when this pattern is activated
	void setupMode(uint8_t mode);

	// Called every frame of the pattern
	void update(uint32_t ms);
};
