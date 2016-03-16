#include <CppUnitLite/TestHarness.h>

#include <stdlib.h>
#include "fakes/FakeFastLED.h"
#include "fakes/FakeSerial.h"
#include "../colortypes.h"

TEST(RGBu, creation) {
	RGBu col;
	CHECK(col.r == 0);
	CHECK(col.g == 0);
	CHECK(col.b == 0);
}