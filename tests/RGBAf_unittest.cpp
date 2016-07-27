#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/colortypes.h"

using namespace LightString;

TEST(RGBAf, creation) {
	RGBAf col;
	EXPECT_RGBAf_EQ(col, 0, 0, 0, 0);
}

TEST(RGBAf, initialization) {
	RGBAf col(0.20, 0.30, 0.40, 0.50);
	EXPECT_RGBAf_EQ(col, 0.20, 0.30, 0.40, 0.50);
}

TEST(RGBAf, boolEval) {
	RGBAf col(0, 0, 0, 0);
	EXPECT_EQ(col, false);

	col = RGBAf(0.10, 0.20, 0.30, 0.40);
	EXPECT_EQ(col, true);
}

TEST(RGBAf, equalsRGBA) {
	RGBAf col;
	RGBAf col2(0.100, 0.150, 0.200, 1);
	
	col = col2;

	EXPECT_RGBAf_EQ(col, 0.100, 0.150, 0.200, 1);
}

TEST(RGBAf, equalsRGB) {
	RGBAf col;
	col.a = 100;
	RGBf col3(5, 6, 7);

	col = col3;

	EXPECT_RGBAf_EQ(col, 5, 6, 7, 100);
}

TEST(RGBAf, addEqualsRBGA) {
	RGBAf col1(20, 30, 40, 50);
	RGBAf col2(10, 11, 12, 13);

	col1 += col2;
	
	EXPECT_RGBAf_EQ(col1, 30, 41, 52, 50);
}

TEST(RGBAf, addEqualsRBG) {
	RGBAf col1(20, 30, 40, 50);
	RGBf col2(10, 11, 12);

	col1 += col2;

	EXPECT_RGBAf_EQ(col1, 30, 41, 52, 50);
}

TEST(RGBAf, addEqualsVal) {
	RGBAf col1(20, 30, 40, 50);
	
	col1 += 20;
	
	EXPECT_RGBAf_EQ(col1, 40, 50, 60, 50);
}

TEST(RGBAf, incrementPrefix) {
	RGBAf col(20, 30, 40, 50);
	RGBAf col2 = ++col;

	EXPECT_RGBAf_EQ(col, 21, 31, 41, 50);
	EXPECT_RGBAf_EQ(col2, 21, 31, 41, 50);
}

TEST(RGBAf, incrementPostfix) {
	RGBAf col(20, 30, 40, 50);
	RGBAf col2 = col++;

	EXPECT_RGBAf_EQ(col, 21, 31, 41, 50);
	EXPECT_RGBAf_EQ(col2, 20, 30, 40, 50);
}

TEST(RGBAf, subEqualsRBG) {
	RGBAf col1(20, 30, 10, 40);
	RGBAf col2(10, 11, 12, 20);

	col1 -= col2;
	
	EXPECT_RGBAf_EQ(col1, 10, 19, -2, 40);
}

TEST(RGBAf, subEqualsVal) {
	RGBAf col1(20, 30, 15, 60);
	
	col1 -= 20;
	
	EXPECT_RGBAf_EQ(col1, 0, 10, -5, 60);
}

TEST(RGBAf, decrementPrefix) {
	RGBAf col(20, 30, 0, 50);
	RGBAf col2 = --col;

	EXPECT_RGBAf_EQ(col, 19, 29, -1, 50);
	EXPECT_RGBAf_EQ(col2, 19, 29, -1, 50);
}

TEST(RGBAf, decrementPostfix) {
	RGBAf col(20, 30, 0, 50);
	RGBAf col2 = col--;

	EXPECT_RGBAf_EQ(col, 19, 29, -1, 50);
	EXPECT_RGBAf_EQ(col2, 20, 30, 0, 50);
}

TEST(RGBAf, multEquals) {
	RGBAf col(20, 30, 40, 50);
	col *= 2.5;

	EXPECT_RGBAf_EQ(col, 50, 75, 100, 50);
}

TEST(RGBAf, mult) {
	RGBAf col(20, 30, 40, 50);
	RGBAf col2 = col * 2.5f;

	EXPECT_RGBAf_EQ(col, 20, 30, 40, 50);
	EXPECT_RGBAf_EQ(col2, 50, 75, 100, 50);
}

TEST(RGBAf, divEquals) {
	RGBAf col(20, 30, 40, 50);
	col /= 5;

	EXPECT_RGBAf_EQ(col, 4, 6, 8, 50);
}

TEST(RGBAf, lerp) {
	RGBAf col1(20, 30, 40, 50);
	RGBAf col2(60, 70, 80, 90);

	col1.lerp(col2, 0.0);

	EXPECT_RGBAf_EQ(col1, 20, 30, 40, 50);

	col1.lerp(col2, 0.5);

	EXPECT_RGBAf_EQ(col1, 40, 50, 60, 70);

	col1.lerp(col2, 1.0);

	EXPECT_RGBAf_EQ(col1, 60, 70, 80, 90);
}

TEST(RGBAf, lerpRGB) {
	RGBAf col1(20, 30, 40, 50);
	RGBf col2(60, 70, 80);

	col1.lerp(col2, 0.0);

	EXPECT_RGBAf_EQ(col1, 20, 30, 40, 50);

	col1.lerp(col2, 0.5);

	EXPECT_RGBAf_EQ(col1, 40, 50, 60, 50);

	col1.lerp(col2, 1.0);

	EXPECT_RGBAf_EQ(col1, 60, 70, 80, 50);
}

TEST(RGBf, lerpRGBA) {
	RGBf col1(20, 30, 40);
	RGBAf col2(60, 70, 80, 1);

	col1.lerp(col2, 0.0);

	EXPECT_RGBf_EQ(col1, 20, 30, 40);

	col1.lerp(col2, 0.5);

	EXPECT_RGBf_EQ(col1, 40, 50, 60);

	col1.lerp(col2, 1.0);

	EXPECT_RGBf_EQ(col1, 60, 70, 80);
}

TEST(RGBAf, getSaturation) {
	RGBAf col(0.10, 0.30, 0.40, 0.8);
	EXPECT_EQ(col.s(), 0.75);

	col = RGBAf(0, 60, 80, 0.8);
	EXPECT_EQ(col.s(), 1.0);
}

TEST(RGBAf, setSaturation) {
	RGBAf col = RGBAf(0.10, 0.30, 0.6, 0.8);
	col.sat(1);

	EXPECT_RGBAf_EQ(col, 0, 0.24, 0.6, 0.8);

	col.sat(0.5);
	EXPECT_RGBAf_EQ(col, 0.3, 0.42, 0.6, 0.8);
}

TEST(RGBAf, getBrightness) {
	RGBAf col(0.20, 0.30, 0.40, 0.8);

	float b = col.bri();
	EXPECT_EQ(b, 0.4f);
}

TEST(RGBAf, setBrightness) {
	RGBAf col(0.20, 0.30, 0.40, 0.8);
	col.bri(0.8);

	EXPECT_RGBAf_EQ(col, 0.4, 0.6, 0.8, 0.8);
}


TEST(RGBAf, maximizeBrightness) {
	RGBAf col = RGBAf(0.4, 0.6, 0.8, 1);
	col.maximizeBrightness();

	EXPECT_RGBAf_EQ(col, 0.5, 0.75, 1.0, 1);
}

TEST(Blending_F, blendCOPYRGBA) {
	TRGBA<float> col1(10, 20, 30, 1);
	TRGBA<float> col2(40, 50, 60, 0.5);

	blendCOPY(col1, col2);

	EXPECT_RGBAf_EQ(col1, 25, 35, 45, 1);
}

TEST(Blending_F, blendCOPYRGB) {
	TRGB<float> col1(10, 20, 30);
	TRGBA<float> col2(40, 50, 60, 0.5);

	blendCOPY(col1, col2);

	EXPECT_RGBf_EQ(col1, 25, 35, 45);
}


TEST(Blending_F, blendADDRGBA) {
	TRGBA<float> col1(10, 20, 30, 1.0);
	TRGBA<float> col2(40, 50, 60, 0.5);

	blendADD(col1, col2);

	EXPECT_RGBAf_EQ(col1, 30, 45, 60, 1);
}

TEST(Blending_F, blendADDRGB) {
	TRGB<float> col1(10, 20, 30);
	TRGBA<float> col2(40, 50, 60, 0.5);

	blendADD(col1, col2);

	EXPECT_RGBf_EQ(col1, 50, 70, 90);
}

