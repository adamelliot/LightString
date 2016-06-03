#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/LightPattern.h"

using namespace LightString;

TEST(PatternCode, equality) {
	PatternCode patternCode1(10, 1, 3);
	PatternCode patternCode2(11, 1, 3);
	PatternCode patternCode3(10, 1, 3);

	EXPECT_FALSE(patternCode1 == patternCode2);
	EXPECT_TRUE(patternCode1 == patternCode3);
}

TEST(LightPattern, initialization) {
	TLightPattern<TRGB> pattern;

	EXPECT_EQ(pattern.getModeCount(), 1);
}
