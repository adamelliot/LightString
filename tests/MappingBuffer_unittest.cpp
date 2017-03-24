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

// -------------- 3d -----------------

TEST(TMappingPixelBuffer3d, creation) {
	TMappingPixelBuffer3d<TRGB, float> buffer(10, 10, 10);

	EXPECT_EQ(buffer.length, 1000);
	EXPECT_TRUE(buffer.shouldDelete);

	TMappingPixelBuffer3d<TRGB, float> buffer2(10, 10, 10, 100);

	EXPECT_EQ(buffer2.length, 100);
	EXPECT_TRUE(buffer2.shouldDelete);
}

TEST(TMappingPixelBuffer3d, indexing) {
	TMappingPixelBuffer3d<TRGB, float> buffer(10, 10, 10);

	EXPECT_EQ(buffer.xyz(2, 2, 2), 222);
	EXPECT_EQ(buffer.xyz(-10, 10, 2), -1);
	EXPECT_EQ(buffer.xyz(10, -10, 2), -1);
}

TEST(TMappingPixelBuffer3d, lineX) {
	TMappingPixelBuffer3d<TRGB, float> buffer(10, 10, 10);

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

TEST(TMappingPixelBuffer3d, lineY) {
	TMappingPixelBuffer3d<TRGB, float> buffer(10, 10, 10);

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

TEST(TMappingPixelBuffer3d, lineZ) {
	TMappingPixelBuffer3d<TRGB, float> buffer(10, 10, 10);

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

TEST(TMappingPixelBuffer3d, lineTo) {
	TMappingPixelBuffer3d<TRGB, float> buffer(10, 10, 10);

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

	PointMapping mapping;

	mapping.addPoint(Point3f(10, 9, 8), 2);
	mapping.addPoint(Point3f(9, 4, 2), 1);

	TMappingPixelBuffer<TRGB, float> buffer(mapping);

	EXPECT_EQ(buffer.getLength(), 3);
	EXPECT_EQ(buffer.width, 11);
	EXPECT_EQ(buffer.height, 10);
	EXPECT_EQ(buffer.depth, 9);
}

TEST(TMappingPixelBuffer, lookupXYZ) {
	PointMapping mapping;

	mapping.addPoint(Point3f(10, 9, 8), 2);
	mapping.addPoint(Point3f(9, 4, 2), 1);
	mapping.addPoint(Point3f(5, 6, 3), 3);

	TMappingPixelBuffer<TRGB, float> buffer(mapping);

	EXPECT_EQ(buffer.xyz(10, 9, 8), 2);
	EXPECT_EQ(buffer.xyz(9, 4, 2), 1);
	EXPECT_EQ(buffer.xyz(5, 3, 1), -1);
	EXPECT_EQ(buffer.xyz(-5, 3, 1), -1);
}

TEST(TMappingPixelBuffer, lookupXY) {
	PointMapping mapping;

	mapping.addPoint(Point2f(10, 9), 2);
	mapping.addPoint(Point2f(9, 4), 1);
	mapping.addPoint(Point2f(5, 6), 3);

	TMappingPixelBuffer<TRGB, float> buffer(mapping);

	EXPECT_EQ(buffer.xy(10, 9), 2);
	EXPECT_EQ(buffer.xy(9, 4), 1);
	EXPECT_EQ(buffer.xy(5, 3), -1);
	EXPECT_EQ(buffer.xy(-5, 3), -1);
}

TEST(TMappingPixelBuffer, setMapping) {
	PointMapping mapping;

	mapping.addPoint(Point3f(10, 9, 8), 2);
	mapping.addPoint(Point3f(9, 4, 2), 1);

	TMappingPixelBuffer<TRGB, float> buffer(mapping);

	EXPECT_EQ(buffer.getLength(), 3);
	EXPECT_EQ(buffer.width, 11);
	EXPECT_EQ(buffer.height, 10);
	EXPECT_EQ(buffer.depth, 9);

	PointMapping mapping2;

	mapping2.addPoint(Point2f(10, 9), 2);
	mapping2.addPoint(Point2f(9, 4), 1);
	mapping2.addPoint(Point2f(5, 6), 3);

	buffer.setMapping(mapping2);

	EXPECT_EQ(buffer.getLength(), 4);
	EXPECT_EQ(buffer.xy(10, 9), 2);
	EXPECT_EQ(buffer.xy(9, 4), 1);
}

/*
TEST(TPixelBuffer, indexAccess) {
	TPixelBuffer<TRGB> buffer(30);

	buffer[0] = RGBu(10, 20, 30);
	RGBu col = buffer[0];

	EXPECT_RGBu_EQ(col, 10, 20, 30);
}

TEST(TPixelBuffer, setPixel) {
	TPixelBuffer<TRGB> buffer(30);

	buffer.setPixel(0, RGBu(10, 20, 30));
	RGBu col = buffer[0];

	EXPECT_RGBu_EQ(buffer[0], 10, 20, 30);
}

TEST(TPixelBuffer, setPixelAA) {
	TPixelBuffer<TRGB> buffer(30);

	buffer[0] = RGBu(20, 40, 60);
	buffer[1] = RGBu(60, 80, 100);

	buffer.setPixelAA(0.75, RGBu(200, 200, 200));

	EXPECT_RGBu_EQ(buffer[0], 65, 80, 95);
	EXPECT_RGBu_EQ(buffer[1], 165, 170, 175);
}

TEST(TPixelBuffer, setPixels) {
	TPixelBuffer<TRGB> buffer(30);

	buffer.setPixels(4, 10, RGBu(10, 20, 30));

	for (int i = 0; i < 10; i++) {
		EXPECT_RGBu_EQ(buffer[i + 4], 10, 20, 30);
	}
}

TEST(TPixelBuffer, setMirroredPixel) {
	TPixelBuffer<TRGB> buffer(30);

	buffer.setMirroredPixel(4, RGBu(10, 20, 30));

	EXPECT_RGBu_EQ(buffer[4], 10, 20, 30);
	EXPECT_RGBu_EQ(buffer[25], 10, 20, 30);
}

TEST(TPixelBuffer, clear) {
	TPixelBuffer<TRGB> buffer(30);

	buffer.setMirroredPixel(4, RGBu(10, 20, 30));
	buffer.clear();

	for (int i = 0; i < 30; i++) {
		EXPECT_RGBu_EQ(buffer[i], 0, 0, 0);
	}
}

TEST(TPixelBuffer, fillColor) {
	TPixelBuffer<TRGB> buffer(30);

	buffer.setMirroredPixel(4, RGBu(10, 20, 30));
	buffer.clear();

	for (int i = 0; i < 30; i++) {
		EXPECT_RGBu_EQ(buffer[i], 0, 0, 0);
	}
}

TEST(TPixelBuffer, fade) {
	TPixelBuffer<TRGB, uint8_t> buffer(30);

	buffer.fillColor(RGBu(100, 80, 60));
	buffer.fade(191);

	for (int i = 0; i < 30; i++) {
		EXPECT_RGBu_EQ(buffer[i], 75, 60, 45);
	}
}














*/