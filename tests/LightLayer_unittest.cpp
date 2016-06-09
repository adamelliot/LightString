#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/LightLayer.h"

using namespace LightString;

TEST(LightLayer, initialization) {
	LightLayer<3, 5> lightLayer;

	EXPECT_EQ(lightLayer.isActive(), false);
}
