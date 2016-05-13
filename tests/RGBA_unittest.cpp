#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/colortypes.h"

using namespace LightString;

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
	
	EXPECT_RGBA_EQ(col1, 30, 41, 52, 50);
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
	
	EXPECT_RGBA_EQ(col1, 40, 50, 60, 50);
}

TEST(RGBAu, incrementPrefix) {
	RGBAu col(20, 30, 40, 50);
	RGBAu col2 = ++col;

	EXPECT_RGBA_EQ(col, 21, 31, 41, 50);
	EXPECT_RGBA_EQ(col2, 21, 31, 41, 50);
}

TEST(RGBAu, incrementPostfix) {
	RGBAu col(20, 30, 40, 50);
	RGBAu col2 = col++;

	EXPECT_RGBA_EQ(col, 21, 31, 41, 50);
	EXPECT_RGBA_EQ(col2, 20, 30, 40, 50);
}

TEST(RGBAu, subEqualsRBG) {
	RGBAu col1(20, 30, 10, 40);
	RGBAu col2(10, 11, 12, 20);

	col1 -= col2;
	
	EXPECT_RGBA_EQ(col1, 10, 19, 0, 40);
}

TEST(RGBAu, subEqualsVal) {
	RGBAu col1(20, 30, 15, 60);
	
	col1 -= 20;
	
	EXPECT_RGBA_EQ(col1, 0, 10, 0, 60);
}

TEST(RGBAu, decrementPrefix) {
	RGBAu col(20, 30, 0, 50);
	RGBAu col2 = --col;

	EXPECT_RGBA_EQ(col, 19, 29, 0, 50);
	EXPECT_RGBA_EQ(col2, 19, 29, 0, 50);
}

TEST(RGBAu, decrementPostfix) {
	RGBAu col(20, 30, 0, 50);
	RGBAu col2 = col--;

	EXPECT_RGBA_EQ(col, 19, 29, 0, 50);
	EXPECT_RGBA_EQ(col2, 20, 30, 0, 50);
}

TEST(RGBAu, multEquals) {
	RGBAu col(20, 30, 40, 50);
	col *= 10;

	EXPECT_RGBA_EQ(col, 200, 255, 255, 50);

	col = RGBAu(20, 30, 40, 50);
	col *= 2;

	EXPECT_RGBA_EQ(col, 40, 60, 80, 50);
}

TEST(RGBAu, divEquals) {
	RGBAu col(20, 30, 40, 50);
	col /= 5;

	EXPECT_RGBA_EQ(col, 4, 6, 8, 50);
}

TEST(RGBAu, modEquals) {
	RGBAu col(20, 30, 40, 50);
	col %= 128; // Scale by half

	EXPECT_RGBA_EQ(col, 10, 15, 20, 50);
}

TEST(RGBAu, lerp) {
	RGBAu col1(20, 30, 40, 50);
	RGBAu col2(60, 70, 80, 90);

	col1.lerp(col2, 0.0);

	EXPECT_RGBA_EQ(col1, 20, 30, 40, 50);

	col1.lerp(col2, 0.5);

	EXPECT_RGBA_EQ(col1, 40, 50, 60, 70);

	col1.lerp(col2, 1.0);

	EXPECT_RGBA_EQ(col1, 60, 70, 80, 90);
}

TEST(RGBAu, lerpRGB) {
	RGBAu col1(20, 30, 40, 50);
	RGBu col2(60, 70, 80);

	col1.lerp(col2, 0.0);

	EXPECT_RGBA_EQ(col1, 20, 30, 40, 50);

	col1.lerp(col2, 0.5);

	EXPECT_RGBA_EQ(col1, 40, 50, 60, 50);

	col1.lerp(col2, 1.0);

	EXPECT_RGBA_EQ(col1, 60, 70, 80, 50);
}

TEST(RGBu, lerpRGBA) {
	RGBu col1(20, 30, 40);
	RGBAu col2(60, 70, 80, 90);

	col1.lerp(col2, 0.0);

	EXPECT_RGB_EQ(col1, 20, 30, 40);

	col1.lerp(col2, 0.5);

	EXPECT_RGB_EQ(col1, 40, 50, 60);

	col1.lerp(col2, 1.0);

	EXPECT_RGB_EQ(col1, 60, 70, 80);
}

TEST(RGBAu, lerp8) {
	RGBAu col1(20, 30, 40, 50);
	RGBAu col2(60, 70, 80, 90);

	col1.lerp8(col2, 0);

	EXPECT_RGBA_EQ(col1, 20, 30, 40, 50);
	col1.lerp8(col2, 192);

	EXPECT_RGBA_EQ(col1, 50, 60, 70, 80);

	col1.lerp8(col2, 255);

	EXPECT_RGBA_EQ(col1, 60, 70, 80, 90);
}

TEST(RGBAu, lerp8RGB) {
	RGBAu col1(20, 30, 40, 50);
	RGBu col2(60, 70, 80);

	col1.lerp8(col2, 0);

	EXPECT_RGBA_EQ(col1, 20, 30, 40, 50);
	col1.lerp8(col2, 192);

	EXPECT_RGBA_EQ(col1, 50, 60, 70, 50);

	col1.lerp8(col2, 255);

	EXPECT_RGBA_EQ(col1, 60, 70, 80, 50);
}

TEST(RGBu, lerp8RGBA) {
	RGBu col1(20, 30, 40);
	RGBAu col2(60, 70, 80, 90);

	col1.lerp8(col2, 0);

	EXPECT_RGB_EQ(col1, 20, 30, 40);
	col1.lerp8(col2, 192);

	EXPECT_RGB_EQ(col1, 50, 60, 70);

	col1.lerp8(col2, 255);

	EXPECT_RGB_EQ(col1, 60, 70, 80);
}

TEST(RGBAu, scale8) {
	RGBAu col(40, 60, 80, 100);
	col.scale8(64); // Scale by quarter

	EXPECT_RGBA_EQ(col, 10, 15, 20, 100);
}

TEST(RGBAu, maximizeBrightness) {
	RGBAu col(40, 60, 100, 70);
	col.maximizeBrightness(200);

	EXPECT_RGBA_EQ(col, 80, 120, 200, 70);

	col = RGBAu(40, 60, 127, 200);
	col.maximizeBrightness();

	EXPECT_RGBA_EQ(col, 80, 120, 254, 200);
}

TEST(Blending_U8, blendCOPYRGBA) {
	TRGBA<uint8_t> col1(10, 20, 30, 255);
	TRGBA<uint8_t> col2(40, 50, 60, 127);

	blendCOPY(col1, col2);

	EXPECT_RGBA_EQ(col1, 25, 35, 45, 255);
}

TEST(Blending_U8, blendCOPYRGB) {
	TRGB<uint8_t> col1(10, 20, 30);
	TRGBA<uint8_t> col2(40, 50, 60, 127);

	blendCOPY(col1, col2);

	EXPECT_RGB_EQ(col1, 25, 35, 45);
}


TEST(Blending_U8, blendADDRGBA) {
	TRGBA<uint8_t> col1(10, 20, 30, 255);
	TRGBA<uint8_t> col2(40, 50, 60, 127);

	blendADD(col1, col2);

	EXPECT_RGBA_EQ(col1, 30, 45, 60, 255);
}

TEST(Blending_U8, blendADDRGB) {
	TRGB<uint8_t> col1(10, 20, 30);
	TRGBA<uint8_t> col2(40, 50, 60, 127);

	blendADD(col1, col2);

	EXPECT_RGB_EQ(col1, 50, 70, 90);
}

