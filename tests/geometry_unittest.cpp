#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/geometry.h"

using namespace LightString;

TEST(TVecBase, scopingAndValueMatching) {
	TVecBase<float, 2> vec;

	vec.x = 10;
	vec.y = 1;

	EXPECT_EQ(vec.x, vec.raw[0]);

	TVecBase<float, 3> vec2;

	vec2.x = 1;
	vec2.y = 1;
	vec2.z = 1;

	TVecBase<float, 4> vec3(1, 1, 1, 15);

	EXPECT_EQ(vec3.w, vec3.raw[3]);
	EXPECT_EQ(15, vec3.raw[3]);
}

TEST(TCuboid, boundPoint) {
	TCuboid<float> bounds(10, 8, 6, 10, 10, 10);
	TPoint<float, 3> pt(5, 5, 5);

	pt = bounds.boundPoint(pt);

	EXPECT_EQ(pt.x, 15);
	EXPECT_EQ(pt.y, 11);
	EXPECT_EQ(pt.z, 7);

	pt = TPoint<float, 3>(24, 25, 22);
	pt = bounds.boundPoint(pt);

	EXPECT_EQ(pt.x, 16);
	EXPECT_EQ(pt.y, 11);
	EXPECT_EQ(pt.z, 10);
}

TEST(TCuboid, includePoint) {
	TCuboid<float> bounds;

	bounds.includePoint(Point3f(10, 10, 10));

	EXPECT_EQ(bounds.width,  10);
	EXPECT_EQ(bounds.height, 10);
	EXPECT_EQ(bounds.depth,  10);
}

TEST(TVec, normalize) {
	TVec<float, 3> v(10, -10, 10);

	auto ret = v.normalize();

	EXPECT_NEAR(ret.x,  0.5774, 0.0001);
	EXPECT_NEAR(ret.y, -0.5774, 0.0001);
	EXPECT_NEAR(ret.z,  0.5774, 0.0001);
}
/*
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
*/