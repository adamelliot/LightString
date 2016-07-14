#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/colortypes.h"

using namespace LightString;

TEST(HSVu, creation) {
	HSVu col;
	EXPECT_HSVu_EQ(col, 0, 0, 0);
}

TEST(HSVu, initialization) {
	HSVu col(0, 255, 255);
	EXPECT_HSVu_EQ(col, 0, 255, 255);
}

TEST(HSVu, equals) {
	HSVu col;
	HSVu col2(100, 150, 200);
	
	col = col2;

	EXPECT_HSVu_EQ(col, 100, 150, 200);
}

TEST(HSVu, toRGB) {
	HSVu col(63, 255, 255);
	RGBu rgb = col.toRGB();

	EXPECT_RGBu_EQ(rgb, 0, 251, 0);
}

