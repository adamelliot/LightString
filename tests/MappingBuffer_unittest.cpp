#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/PixelBuffer.h"

using namespace LightString;

// -------------- 2d -----------------

TEST(TMappingPixelBuffer2d, creation) {
	TMappingPixelBuffer2d<TRGB, uint8_t> buffer(20, 20);

	EXPECT_EQ(buffer.length, 400);
	EXPECT_TRUE(buffer.shouldDelete);

	TMappingPixelBuffer2d<TRGB, uint8_t> buffer2(20, 20, 100);

	EXPECT_EQ(buffer2.length, 100);
	EXPECT_TRUE(buffer2.shouldDelete);
}

TEST(TMappingPixelBuffer2d, indexing) {
	TMappingPixelBuffer2d<TRGB, uint8_t> buffer(20, 20);

	EXPECT_EQ(buffer.xy(10, 10), 210);
	EXPECT_EQ(buffer.xy(-10, 10), -1);
	EXPECT_EQ(buffer.xy(10, -10), -1);
}

TEST(TMappingPixelBuffer2d, dummyPixelAccess) {
	TMappingPixelBuffer2d<TRGB, float> buffer(10, 10);

	buffer[-1] = RGBf(0.3, 1, 0.2);
	EXPECT_RGBf_EQ(buffer[-1], 0.3, 1, 0.2);	
}

// -------------- 3d -----------------

TEST(TPixelBuffer3d, creation) {
	TPixelBuffer3d<TRGB, float> buffer(10, 10, 10);

	EXPECT_EQ(buffer.length, 1000);
	EXPECT_TRUE(buffer.shouldDelete);

	TPixelBuffer3d<TRGB, float> buffer2(10, 10, 10, 100);

	EXPECT_EQ(buffer2.length, 100);
	EXPECT_TRUE(buffer2.shouldDelete);
}

TEST(TPixelBuffer3d, indexing) {
	TPixelBuffer3d<TRGB, float> buffer(10, 10, 10);

	EXPECT_EQ(buffer.xyz(2, 2, 2), 222);
	EXPECT_EQ(buffer.xyz(-10, 10, 2), -1);
	EXPECT_EQ(buffer.xyz(10, -10, 2), -1);
}

TEST(TPixelBuffer3d, dummyPixelAccess) {
	TPixelBuffer3d<TRGB, float> buffer(10, 10, 10);

	buffer.pixels[-1] = RGBf(0.3, 1, 0.2);
	EXPECT_RGBf_EQ(buffer[-1], 0.3, 1, 0.2);	
}

TEST(TPixelBuffer3d, lineX) {
	TPixelBuffer3d<TRGB, float> buffer(10, 10, 10);

	buffer.clear();
	buffer.lineX(2, 2, 2, 3, HTML::White);

	EXPECT_RGBf_EQ(buffer[222], 1.0f, 1.0f, 1.0f);
	EXPECT_RGBf_EQ(buffer[223], 1.0f, 1.0f, 1.0f);
	EXPECT_RGBf_EQ(buffer[224], 1.0f, 1.0f, 1.0f);

	buffer.clear();
	buffer.lineX(2, 2, 2, -3, HTML::Red);

	EXPECT_RGBf_EQ(buffer[222], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[221], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[220], 1.0f, 0.0f, 0.0f);
}

TEST(TPixelBuffer3d, lineY) {
	TPixelBuffer3d<TRGB, float> buffer(10, 10, 10);

	buffer.clear();
	buffer.lineY(2, 2, 2, 3, HTML::White);

	EXPECT_RGBf_EQ(buffer[222], 1.0f, 1.0f, 1.0f);
	EXPECT_RGBf_EQ(buffer[232], 1.0f, 1.0f, 1.0f);
	EXPECT_RGBf_EQ(buffer[242], 1.0f, 1.0f, 1.0f);

	buffer.clear();
	buffer.lineY(2, 2, 2, -3, HTML::Red);

	EXPECT_RGBf_EQ(buffer[222], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[212], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[202], 1.0f, 0.0f, 0.0f);
}

TEST(TPixelBuffer3d, lineZ) {
	TPixelBuffer3d<TRGB, float> buffer(10, 10, 10);

	buffer.clear();
	buffer.lineZ(2, 2, 2, 3, HTML::White);

	EXPECT_RGBf_EQ(buffer[222], 1.0f, 1.0f, 1.0f);
	EXPECT_RGBf_EQ(buffer[322], 1.0f, 1.0f, 1.0f);
	EXPECT_RGBf_EQ(buffer[422], 1.0f, 1.0f, 1.0f);

	buffer.clear();
	buffer.lineZ(2, 2, 2, -3, HTML::Red);

	EXPECT_RGBf_EQ(buffer[222], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[122], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[ 22], 1.0f, 0.0f, 0.0f);
}

TEST(TPixelBuffer3d, lineTo) {
	TPixelBuffer3d<TRGB, float> buffer(10, 10, 10);

	buffer.clear();
	buffer.lineTo(2, 2, 2, 5, 5, 5, HTML::Red);

	EXPECT_RGBf_EQ(buffer[222], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[333], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[444], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[555], 1.0f, 0.0f, 0.0f);

	buffer.clear();
	buffer.lineTo(2, 2, 2, 5, 5, 2, HTML::Red);

	EXPECT_RGBf_EQ(buffer[222], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[233], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[244], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[255], 1.0f, 0.0f, 0.0f);

	buffer.clear();
	buffer.lineTo(2, 2, 2, 5, 5, 3, HTML::Red);

	EXPECT_RGBf_EQ(buffer[222], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[233], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[344], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[355], 1.0f, 0.0f, 0.0f);

	buffer.clear();
	buffer.lineTo(6, 6, 6, 2, 6, 2, HTML::Red);

	EXPECT_RGBf_EQ(buffer[666], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[565], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[464], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[363], 1.0f, 0.0f, 0.0f);
	EXPECT_RGBf_EQ(buffer[262], 1.0f, 0.0f, 0.0f);

	buffer.clear();
	buffer.lineTo(6, 6, 6, 6, 6, 6, HTML::Red);

	EXPECT_RGBf_EQ(buffer[666], 1.0f, 0.0f, 0.0f);
}

// -------------- Generic Mapping -----------------

TEST(TMappingPixelBuffer, creation) {

	TPointMapping<float> mapping;

	mapping.addPoint(Point3f(10, 9, 8), 2);
	mapping.addPoint(Point3f(9, 4, 2), 1);

	TMappingPixelBuffer<TRGB, float> buffer(mapping);

	EXPECT_EQ(buffer.getLength(), 3);
	EXPECT_EQ(buffer.width, 10);
	EXPECT_EQ(buffer.height, 9);
	EXPECT_EQ(buffer.depth, 8);
}

TEST(TMappingPixelBuffer3d, lookupXYZ) {
	TPointMapping<int> mapping;

	mapping.addPoint(Point3i(10, 9, 8), 2);
	mapping.addPoint(Point3i(9, 4, 2), 1);
	mapping.addPoint(Point3i(5, 6, 3), 3);

	TMappingPixelBuffer3d<TRGB, float> buffer(mapping);

	EXPECT_EQ(buffer.xyz(10, 9, 8), 2);
	EXPECT_EQ(buffer.xyz(9, 4, 2), 1);
	EXPECT_EQ(buffer.xyz(5, 3, 1), -1);
	EXPECT_EQ(buffer.xyz(-5, 3, 1), -1);
}

TEST(TMappingPixelBuffer, setMapping) {
	TPointMapping<float> mapping;

	mapping.addPoint(Point3f(10, 9, 8), 2);
	mapping.addPoint(Point3f(9, 4, 2), 1);

	TMappingPixelBuffer<TRGB, float> buffer(mapping);

	EXPECT_EQ(buffer.getLength(), 3);
	EXPECT_EQ(buffer.width, 10);
	EXPECT_EQ(buffer.height, 9);
	EXPECT_EQ(buffer.depth, 8);

	TPointMapping<float> mapping2;

	mapping2.addPoint(Point2f(10, 9), 2);
	mapping2.addPoint(Point2f(9, 4), 1);
	mapping2.addPoint(Point2f(5, 6), 3);

	buffer.setMapping(mapping2);

	EXPECT_EQ(buffer.getSize(), 4);
}
