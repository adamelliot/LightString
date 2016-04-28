#include "gtest/gtest.h"

#include <stdlib.h>
#include "fakes/FakeFastLED.h"
#include "fakes/FakeSerial.h"

#include "../src/colortypes.h"

#define CHECK_RGB(COL, R, G, B) { EXPECT_EQ(COL.r, R); EXPECT_EQ(COL.g, G); EXPECT_EQ(COL.b, B); };

// ------------- RGBu -----------------

TEST(RGBu, creation) {
	RGBu col;
	CHECK_RGB(col, 0, 0, 0);
}

TEST(RGBu, initialization) {
	RGBu col(20, 30, 40);
	CHECK_RGB(col, 20, 30, 40);
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

	CHECK_RGB(col, 100, 150, 200);
}

TEST(RGBu, addEqualsRBG) {
	RGBu col1(20, 30, 40);
	RGBu col2(10, 11, 12);

	col1 += col2;
	
	CHECK_RGB(col1, 30, 41, 52);
}

TEST(RGBu, addEqualsVal) {
	RGBu col1(20, 30, 40);
	
	col1 += 20;
	
	CHECK_RGB(col1, 40, 50, 60);
}

TEST(RGBu, incrementPrefix) {
	RGBu col(20, 30, 40);
	RGBu col2 = ++col;

	CHECK_RGB(col, 21, 31, 41);
	CHECK_RGB(col2, 21, 31, 41);
}

TEST(RGBu, incrementPostfix) {
	RGBu col(20, 30, 40);
	RGBu col2 = col++;

	CHECK_RGB(col, 21, 31, 41);
	CHECK_RGB(col2, 20, 30, 40);
}

TEST(RGBu, multEquals) {
	RGBu col(20, 30, 40);
	col *= 10;

	CHECK_RGB(col, 200, 255, 255);

	col = RGBu(20, 30, 40);
	col *= 2;

	CHECK_RGB(col, 40, 60, 80);
}

TEST(RGBu, modEquals) {
	RGBu col(20, 30, 40);
	col %= 128; // Scale by half

	CHECK_RGB(col, 10, 15, 20);
}

TEST(RGBu, lerp) {
	RGBu col1(20, 30, 40);
	RGBu col2(60, 70, 80);

	col1.lerp(col2, 0.5);

	CHECK_RGB(col1, 40, 50, 60);
}

TEST(RGBu, lerp8) {
	RGBu col1(20, 30, 40);
	RGBu col2(60, 70, 80);

	col1.lerp8(col2, 192);

	CHECK_RGB(col1, 50, 60, 70);
}

TEST(RGBu, scale8) {
	RGBu col(40, 60, 80);
	col %= 64; // Scale by quarter

	CHECK_RGB(col, 10, 15, 20);
}

TEST(RGBu, maximizeBrightness) {
	RGBu col(40, 60, 100);
	col.maximizeBrightness(200);

	CHECK_RGB(col, 80, 120, 200);

	col = RGBu(40, 60, 127);
	col.maximizeBrightness();	

	CHECK_RGB(col, 80, 120, 254);
}

// ------------- RGBAu -----------------

