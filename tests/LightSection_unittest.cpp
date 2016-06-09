#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/LightSection.h"

using namespace LightString;

TEST(LightSection, initialization) {
	LightSection<TRGB, uint8_t, 3, 5, 1> lightSection;

	EXPECT_TRUE(lightSection.outputBuffer == NULL);
}
