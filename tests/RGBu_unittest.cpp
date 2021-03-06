#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/colortypes.h"

using namespace LightString;

TEST(RGBu, creation) {
	RGBu col;
	EXPECT_RGBu_EQ(col, 0, 0, 0);
}

TEST(RGBu, initialization) {
	RGBu col(20, 30, 40);
	EXPECT_RGBu_EQ(col, 20, 30, 40);
}

TEST(RGBu, boolEval) {
	RGBu col(0, 0, 0);
	EXPECT_EQ(col, false);

	col = RGBu(10, 20, 30);
	EXPECT_EQ(col, true);
}

TEST(RGBu, equals) {
	RGBu col;
	RGBu col2(100, 150, 200);
	
	col = col2;

	EXPECT_RGBu_EQ(col, 100, 150, 200);
}

TEST(RGBu, equalsHSV) {
	HSVu col(127, 255, 255);
	RGBu rgb = col;

	EXPECT_RGBu_EQ(rgb, 0, 0, 251);
}
/*
TEST(RGBu, toHSV) {
	RGBu col(255, 0, 0);
	HSVu hsv = col.toHSV();

	EXPECT_HSVu_EQ(hsv, 0, 255, 255);
}*/

TEST(RGBu, addEqualsRBG) {
	RGBu col1(20, 30, 40);
	RGBu col2(10, 11, 12);

	col1 += col2;
	
	EXPECT_RGBu_EQ(col1, 30, 41, 52);
}

TEST(RGBu, addEqualsVal) {
	RGBu col1(20, 30, 40);
	
	col1 += 20;
	
	EXPECT_RGBu_EQ(col1, 40, 50, 60);
}

TEST(RGBu, incrementPrefix) {
	RGBu col(20, 30, 40);
	RGBu col2 = ++col;

	EXPECT_RGBu_EQ(col, 21, 31, 41);
	EXPECT_RGBu_EQ(col2, 21, 31, 41);
}

TEST(RGBu, incrementPostfix) {
	RGBu col(20, 30, 40);
	RGBu col2 = col++;

	EXPECT_RGBu_EQ(col, 21, 31, 41);
	EXPECT_RGBu_EQ(col2, 20, 30, 40);
}

TEST(RGBu, subEqualsRBG) {
	RGBu col1(20, 30, 10);
	RGBu col2(10, 11, 12);

	col1 -= col2;
	
	EXPECT_RGBu_EQ(col1, 10, 19, 0);
}

TEST(RGBu, subEqualsVal) {
	RGBu col1(20, 30, 40);
	
	col1 -= 20;
	
	EXPECT_RGBu_EQ(col1, 0, 10, 20);
}

TEST(RGBu, decrementPrefix) {
	RGBu col(20, 30, 40);
	RGBu col2 = --col;

	EXPECT_RGBu_EQ(col, 19, 29, 39);
	EXPECT_RGBu_EQ(col2, 19, 29, 39);
}

TEST(RGBu, decrementPostfix) {
	RGBu col(20, 30, 40);
	RGBu col2 = col--;

	EXPECT_RGBu_EQ(col, 19, 29, 39);
	EXPECT_RGBu_EQ(col2, 20, 30, 40);
}

TEST(RGBu, multEquals) {
	RGBu col(20, 30, 40);
	col *= 10;

	EXPECT_RGBu_EQ(col, 200, 255, 255);

	col = RGBu(20, 30, 40);
	col *= 2;

	EXPECT_RGBu_EQ(col, 40, 60, 80);
}

TEST(RGBu, mult) {
	RGBu col(20, 30, 40);
	RGBu col2 = col * (uint8_t)10;

	EXPECT_RGBu_EQ(col, 20, 30, 40);
	EXPECT_RGBu_EQ(col2, 200, 255, 255);
}

TEST(RGBu, divEquals) {
	RGBu col(20, 30, 40);
	col /= 5;

	EXPECT_RGBu_EQ(col, 4, 6, 8);
}

TEST(RGBu, modEquals) {
	RGBu col(20, 30, 40);
	col %= 128; // Scale by half

	EXPECT_RGBu_EQ(col, 10, 15, 20);
}

TEST(RGBu, lerp) {
	RGBu col1(20, 30, 40);
	RGBu col2(60, 70, 80);

	col1.lerp(col2, 0.5);

	EXPECT_RGBu_EQ(col1, 40, 50, 60);
}

TEST(RGBu, lerp8) {
	RGBu col1(20, 30, 40);
	RGBu col2(60, 70, 80);

	col1.lerp8(col2, 192);

	EXPECT_RGBu_EQ(col1, 50, 60, 70);
}

TEST(RGBu, scale8) {
	RGBu col(40, 60, 80);
	col.scale8(64); // Scale by quarter

	EXPECT_RGBu_EQ(col, 10, 15, 20);
}

TEST(RGBu, fade) {
	RGBu col(40, 60, 80);
	col.fade(64); // Scale by quarter

	EXPECT_RGBu_EQ(col, 10, 15, 20);
}

TEST(RGBu, fadeCopy) {
	RGBu col(40, 60, 80);
	RGBu ret = col.fadeCopy(64); // Scale by quarter

	EXPECT_RGBu_EQ(col, 40, 60, 80);
	EXPECT_RGBu_EQ(ret, 10, 15, 20);
}

TEST(RGBu, getSaturation) {
	RGBu col(20, 60, 80);
	EXPECT_EQ(col.s(), 191);

	col = RGBu(0, 60, 80);
	EXPECT_EQ(col.s(), 255);
}

TEST(RGBu, setSaturation) {
	RGBu col = RGBu(10, 30, 110);
	col.sat(255);

	EXPECT_RGBu_EQ(col, 0, 22, 110);

	col.sat(127);
	EXPECT_RGBu_EQ(col, 55, 66, 110);
}

TEST(RGBu, getBrightness) {
	RGBu col(40, 60, 100);

	EXPECT_EQ(col.bri(), 100);
}

TEST(RGBu, setBrightness) {
	RGBu col(40, 60, 100);
	col.bri(200);

	EXPECT_RGBu_EQ(col, 80, 120, 200);
}

TEST(RGBu, maximizeBrightness) {
	RGBu col = RGBu(40, 60, 127);
	col.maximizeBrightness();	

	EXPECT_RGBu_EQ(col, 80, 120, 254);
}
