#include "gtest/gtest.h"

#include <stdlib.h>

#include "../src/colortypes.h"

#define EXPECT_RGB_EQ(COL, R, G, B) { EXPECT_EQ(COL.r, R); EXPECT_EQ(COL.g, G); EXPECT_EQ(COL.b, B); };

// ------------- RGBu -----------------

TEST(RGBu, creation) {
	RGBu col;
	EXPECT_RGB_EQ(col, 0, 0, 0);
}

TEST(RGBu, initialization) {
	RGBu col(20, 30, 40);
	EXPECT_RGB_EQ(col, 20, 30, 40);
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

	EXPECT_RGB_EQ(col, 100, 150, 200);
}

TEST(RGBu, addEqualsRBG) {
	RGBu col1(20, 30, 40);
	RGBu col2(10, 11, 12);

	col1 += col2;
	
	EXPECT_RGB_EQ(col1, 30, 41, 52);
}

TEST(RGBu, addEqualsVal) {
	RGBu col1(20, 30, 40);
	
	col1 += 20;
	
	EXPECT_RGB_EQ(col1, 40, 50, 60);
}

TEST(RGBu, incrementPrefix) {
	RGBu col(20, 30, 40);
	RGBu col2 = ++col;

	EXPECT_RGB_EQ(col, 21, 31, 41);
	EXPECT_RGB_EQ(col2, 21, 31, 41);
}

TEST(RGBu, incrementPostfix) {
	RGBu col(20, 30, 40);
	RGBu col2 = col++;

	EXPECT_RGB_EQ(col, 21, 31, 41);
	EXPECT_RGB_EQ(col2, 20, 30, 40);
}

TEST(RGBu, subEqualsRBG) {
	RGBu col1(20, 30, 10);
	RGBu col2(10, 11, 12);

	col1 -= col2;
	
	EXPECT_RGB_EQ(col1, 10, 19, 0);
}

TEST(RGBu, subEqualsVal) {
	RGBu col1(20, 30, 40);
	
	col1 -= 20;
	
	EXPECT_RGB_EQ(col1, 0, 10, 20);
}

TEST(RGBu, decrementPrefix) {
	RGBu col(20, 30, 40);
	RGBu col2 = --col;

	EXPECT_RGB_EQ(col, 19, 29, 39);
	EXPECT_RGB_EQ(col2, 19, 29, 39);
}

TEST(RGBu, decrementPostfix) {
	RGBu col(20, 30, 40);
	RGBu col2 = col--;

	EXPECT_RGB_EQ(col, 19, 29, 39);
	EXPECT_RGB_EQ(col2, 20, 30, 40);
}

TEST(RGBu, multEquals) {
	RGBu col(20, 30, 40);
	col *= 10;

	EXPECT_RGB_EQ(col, 200, 255, 255);

	col = RGBu(20, 30, 40);
	col *= 2;

	EXPECT_RGB_EQ(col, 40, 60, 80);
}

TEST(RGBu, modEquals) {
	RGBu col(20, 30, 40);
	col %= 128; // Scale by half

	EXPECT_RGB_EQ(col, 10, 15, 20);
}

TEST(RGBu, divEquals) {
	RGBu col(20, 30, 40);
	col /= 5;

	EXPECT_RGB_EQ(col, 4, 6, 8);
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

// ------------- RGBAu -----------------

