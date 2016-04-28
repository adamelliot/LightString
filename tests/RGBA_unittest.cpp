#include "gtest/gtest.h"

#include <stdlib.h>
#include "fakes/FakeFastLED.h"

#include "../src/colortypes.h"

#define EXPECT_RGBA_EQ(COL, R, G, B, A) { EXPECT_EQ(COL.r, R); EXPECT_EQ(COL.g, G); EXPECT_EQ(COL.b, B); EXPECT_EQ(COL.a, A); };

// ------------- RGBu -----------------

TEST(RGBAu, creation) {
	RGBAu col;
	EXPECT_RGBA_EQ(col, 0, 0, 0, 0);
}

TEST(RGBAu, initialization) {
	RGBAu col(20, 30, 40, 50);
	EXPECT_RGBA_EQ(col, 20, 30, 40, 50);
}

TEST(RGBAu, boolEval) {
	RGBAu col(0, 0, 0, 0);
	EXPECT_EQ(col, false);

	col = RGBAu(10, 20, 30, 40);
	EXPECT_EQ(col, true);
}

TEST(RGBAu, equalsRGBA) {
	RGBAu col;
	RGBAu col2(100, 150, 200, 255);
	
	col = col2;

	EXPECT_RGBA_EQ(col, 100, 150, 200, 255);
}

TEST(RGBAu, equalsRGB) {
	RGBAu col;
	col.a = 100;
	RGBu col3(5, 6, 7);

	col = col3;

	EXPECT_RGBA_EQ(col, 5, 6, 7, 100);
}

TEST(RGBAu, addEqualsRBGA) {
	RGBAu col1(20, 30, 40, 50);
	RGBAu col2(10, 11, 12, 13);

	col1 += col2;
	
	EXPECT_RGBA_EQ(col1, 30, 41, 52, 63);
}

TEST(RGBAu, addEqualsRBG) {
	RGBAu col1(20, 30, 40, 50);
	RGBu col2(10, 11, 12);

	col1 += col2;
	
	EXPECT_RGBA_EQ(col1, 30, 41, 52, 50);
}

TEST(RGBAu, addEqualsVal) {
	RGBAu col1(20, 30, 40, 50);
	
	col1 += 20;
	
	EXPECT_RGBA_EQ(col1, 40, 50, 60, 70);
}

TEST(RGBAu, incrementPrefix) {
	RGBAu col(20, 30, 40, 50);
	RGBAu col2 = ++col;

	EXPECT_RGBA_EQ(col, 21, 31, 41, 51);
	EXPECT_RGBA_EQ(col2, 21, 31, 41, 51);
}

TEST(RGBAu, incrementPostfix) {
	RGBAu col(20, 30, 40, 50);
	RGBAu col2 = col++;

	EXPECT_RGBA_EQ(col, 21, 31, 41, 51);
	EXPECT_RGBA_EQ(col2, 20, 30, 40, 50);
}

TEST(RGBAu, multEquals) {
	RGBAu col(20, 30, 40, 50);
	col *= 10;

	EXPECT_RGBA_EQ(col, 200, 255, 255, 255);

	col = RGBAu(20, 30, 40, 50);
	col *= 2;

	EXPECT_RGBA_EQ(col, 40, 60, 80, 100);
}
/*
TEST(RGBu, modEquals) {
	RGBu col(20, 30, 40);
	col %= 128; // Scale by half

	EXPECT_RGB_EQ(col, 10, 15, 20);
}

TEST(RGBu, lerp) {
	RGBu col1(20, 30, 40);
	RGBu col2(60, 70, 80);

	col1.lerp(col2, 0.5);

	EXPECT_RGB_EQ(col1, 40, 50, 60);
}

TEST(RGBu, lerp8) {
	RGBu col1(20, 30, 40);
	RGBu col2(60, 70, 80);

	col1.lerp8(col2, 192);

	EXPECT_RGB_EQ(col1, 50, 60, 70);
}

TEST(RGBu, scale8) {
	RGBu col(40, 60, 80);
	col %= 64; // Scale by quarter

	EXPECT_RGB_EQ(col, 10, 15, 20);
}

TEST(RGBu, maximizeBrightness) {
	RGBu col(40, 60, 100);
	col.maximizeBrightness(200);

	EXPECT_RGB_EQ(col, 80, 120, 200);

	col = RGBu(40, 60, 127);
	col.maximizeBrightness();	

	EXPECT_RGB_EQ(col, 80, 120, 254);
}
*/