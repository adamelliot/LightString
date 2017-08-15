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

TEST(TVecBase, distance) {
	TPoint<float, 3> a(10, 10, 10);
	TPoint<float, 3> b(2, 2, 2);

	auto ret = a.distance(b);

	EXPECT_NEAR(ret, 13.85, 0.01);
}

TEST(TVecBase, manhattanDistance) {
	TPoint<float, 3> a(10, 10, 10);
	TPoint<float, 3> b(2, 2, 2);

	auto ret = a.manhattanDistance(b);

	EXPECT_NEAR(ret, 24, 0.001);
}

