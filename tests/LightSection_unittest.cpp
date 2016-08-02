#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/LightSection.h"

using namespace LightString;

TEST(LightSection, initialization) {
	LightSection<TRGB, uint8_t> lightSection;

	EXPECT_TRUE(lightSection.outputBuffer == NULL);
}

TEST(LightSection, ensureLayerExists) {
	LightSection<TRGB, uint8_t> lightSection;

	lightSection.ensureLayerExists(4);

	auto layer = lightSection.getLayer(4);

	EXPECT_EQ(layer->getLayerID(), 4);
}