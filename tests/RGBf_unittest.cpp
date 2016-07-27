#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/colortypes.h"

using namespace LightString;

TEST(RGBf, creation) {
	RGBf col;
	EXPECT_RGBf_EQ(col, 0, 0, 0);
}

TEST(RGBf, initialization) {
	RGBf col(0.5, 0.6, 0.7);
	EXPECT_RGBf_EQ(col, 0.5, 0.6, 0.7);
}

TEST(RGBf, boolEval) {
	RGBf col(0, 0, 0);
	EXPECT_EQ(col, false);

	col = RGBf(0.5, 0.6, 0.7);
	EXPECT_EQ(col, true);
}

TEST(RGBf, equals) {
	RGBf col;
	RGBf col2(0.4, 0.8, 0.9);
	
	col = col2;

	EXPECT_RGBf_EQ(col, 0.4, 0.8, 0.9);
}

TEST(RGBf, equalsHSV) {
	HSVf col(240, 1, 1);
	RGBf rgb = col;

	EXPECT_RGBf_EQ(rgb, 0, 0, 1);
}

TEST(RGBf, toHSV) {
	RGBf col(1, 0, 0);
	HSVf hsv = col.toHSV();

	EXPECT_HSVu_EQ(hsv, 0, 1, 1);
}

TEST(RGBf, addEqualsRBG) {
	RGBf col1(0.20, 0.30, 0.40);
	RGBf col2(0.10, 0.11, 0.12);

	col1 += col2;
	
	EXPECT_RGBf_EQ(col1, 0.30, 0.41, 0.52);
}

TEST(RGBf, addEqualsVal) {
	RGBf col1(0.20, 0.30, 0.40);
	
	col1 += 0.20;
	
	EXPECT_RGBf_EQ(col1, 0.40, 0.50, 0.60);
}

TEST(RGBf, incrementPrefix) {
	RGBf col(0.20, 0.30, 0.40);
	RGBf col2 = ++col;

	EXPECT_RGBf_EQ(col, 1.2, 1.3, 1.4);
	EXPECT_RGBf_EQ(col2, 1.2, 1.3, 1.4);
}

TEST(RGBf, incrementPostfix) {
	RGBf col(0.20, 0.30, 0.40);
	RGBf col2 = col++;

	EXPECT_RGBf_EQ(col, 1.20, 1.30, 1.40);
	EXPECT_RGBf_EQ(col2, 0.20, 0.30, 0.40);
}

TEST(RGBf, subEqualsRBG) {
	RGBf col1(0.20, 0.30, 0.10);
	RGBf col2(0.10, 0.11, 0.12);

	col1 -= col2;
	
	EXPECT_RGBf_EQ(col1, 0.10, 0.19, -0.02);
}

TEST(RGBf, subEqualsVal) {
	RGBf col1(0.20, 0.30, 0.40);
	
	col1 -= 0.20;
	
	EXPECT_RGBf_EQ(col1, 0, 0.10, 0.20);
}

TEST(RGBf, decrementPrefix) {
	RGBf col(0.20, 0.30, 0.40);
	RGBf col2 = --col;

	EXPECT_RGBf_EQ(col, -0.8, -0.7, -0.6);
	EXPECT_RGBf_EQ(col2, -0.8, -0.7, -0.6);
}

TEST(RGBf, decrementPostfix) {
	RGBf col(0.20, 0.30, 0.40);
	RGBf col2 = col--;

	EXPECT_RGBf_EQ(col, -0.8, -0.7, -0.6);
	EXPECT_RGBf_EQ(col2, 0.20, 0.30, 0.40);
}

TEST(RGBf, multEquals) {
	RGBf col(0.20, 0.30, 0.40);
	col *= 2.5;

	EXPECT_RGBf_EQ(col, 0.5, 0.75, 1.0);
}

TEST(RGBf, mult) {
	RGBf col(0.20, 0.30, 0.40);
	RGBf col2 = col * 2.5f;

	EXPECT_RGBf_EQ(col, 0.20, 0.30, 0.40);
	EXPECT_RGBf_EQ(col2, 0.5, 0.75, 1.0);
}

TEST(RGBf, divEquals) {
	RGBf col(0.20, 0.30, 0.40);
	col /= 5;

	EXPECT_RGBf_EQ(col, 0.04, 0.06, 0.08);
}

TEST(RGBf, lerp) {
	RGBf col1(0.20, 0.30, 0.40);
	RGBf col2(0.60, 0.70, 0.80);

	col1.lerp(col2, 0.5);

	EXPECT_RGBf_EQ(col1, 0.40, 0.50, 0.60);
}

TEST(RGBf, brightness) {
	RGBf col(0.20, 0.30, 0.40);

	float b = col.bri();
	EXPECT_EQ(b, 0.4f);

	col.bri(0.8);

	EXPECT_RGBf_EQ(col, 0.4, 0.6, 0.8);
}

TEST(RGBf, maximizeBrightness) {
	RGBf col = RGBf(0.20, 0.30, 0.40);
	col.maximizeBrightness();	

	EXPECT_RGBf_EQ(col, 0.5, 0.75, 1.0);
}


