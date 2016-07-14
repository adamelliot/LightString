#pragma once

#include "LightString.h"

#define WHEEL_PATTERN 0x10

using namespace LightString;

class WheelPattern : public LightPattern {
protected:

public:
	WheelPattern()
		: LightPattern(1) {}

	uint8_t getPatternID() { return WHEEL_PATTERN; }

	void setupMode(uint8_t mode);
	void update(uint32_t ms);
};
