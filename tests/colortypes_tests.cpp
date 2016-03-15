#include <CppUnitLite/TestHarness.h>

#include <stdlib.h>
#include "fake_FastLED.h"
#include "fake_Serial.h"
#include "../colortypes.h"

TEST(RGBu, creation) {
	RGBu col;
	CHECK(col.r == 0);
	CHECK(col.g == 0);
	CHECK(col.b == 0);
}