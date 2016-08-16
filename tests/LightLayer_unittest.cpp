#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/LightLayer.h"

using namespace LightString;

TEST(LightLayer, initialization) {
	LightLayer<float> lightLayer;

	EXPECT_EQ(lightLayer.isActive(), false);
	EXPECT_EQ(lightLayer.getOpacity(), 1.0f);
}
