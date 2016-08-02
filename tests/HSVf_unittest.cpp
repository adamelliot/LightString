#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/colortypes.h"

using namespace LightString;

TEST(HSVf, creation) {
	HSVu col;
	EXPECT_HSVu_EQ(col, 0, 0, 0);
}

TEST(HSVf, initialization) {
	HSVu col(0, 255, 255);
	EXPECT_HSVu_EQ(col, 0, 255, 255);
}

TEST(HSVf, equals) {
	HSVf col;
	HSVf col2(60, 1, 1);
	
	col = col2;

	EXPECT_HSVf_EQ(col, 60, 1, 1);
}

TEST(HSVf, toRGB) {
	HSVf col(120, 1, 1);
	RGBf rgb = col.toRGB();

	EXPECT_RGBf_EQ(rgb, 0, 1, 0);
}

TEST(HSVf, toRGBA) {
	HSVf col(120, 1, 1);
	RGBAf rgb = col;

	EXPECT_RGBAf_EQ(rgb, 0, 1, 0, 1);
}
