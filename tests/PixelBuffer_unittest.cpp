#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/PixelBuffer.h"

using namespace LightString;

TEST(TPixelBuffer, creation) {
	TPixelBuffer<TRGB, uint8_t> buffer(30);

	EXPECT_EQ(buffer.length, 30);
	EXPECT_TRUE(buffer.shouldDelete);
}

TEST(TPixelBuffer, creationExternalMemory) {
	RGBu rawBuff[30];
	TPixelBuffer<TRGB> buffer(rawBuff, 30);

	EXPECT_EQ(buffer.length, 30);
	EXPECT_EQ(buffer.pixels, rawBuff);
	EXPECT_FALSE(buffer.shouldDelete);
}

TEST(TPixelBuffer, indexAccess) {
	TPixelBuffer<TRGB> buffer(30);

	buffer[0] = RGBu(10, 20, 30);
	RGBu col = buffer[0];

	EXPECT_RGB_EQ(col, 10, 20, 30);
}

TEST(TPixelBuffer, setPixel) {
	TPixelBuffer<TRGB> buffer(30);

	buffer.setPixel(0, RGBu(10, 20, 30));
	RGBu col = buffer[0];

	EXPECT_RGB_EQ(buffer[0], 10, 20, 30);
}

TEST(TPixelBuffer, setPixelAA) {
	TPixelBuffer<TRGB> buffer(30);

	buffer[0] = RGBu(20, 40, 60);
	buffer[1] = RGBu(60, 80, 100);

	buffer.setPixelAA(0.75, RGBu(200, 200, 200));

	EXPECT_RGB_EQ(buffer[0], 65, 80, 95);
	EXPECT_RGB_EQ(buffer[1], 165, 170, 175);
}

TEST(TPixelBuffer, setPixels) {
	TPixelBuffer<TRGB> buffer(30);

	buffer.setPixels(4, 10, RGBu(10, 20, 30));

	for (int i = 0; i < 10; i++) {
		EXPECT_RGB_EQ(buffer[i + 4], 10, 20, 30);
	}
}

TEST(TPixelBuffer, setMirroredPixel) {
	TPixelBuffer<TRGB> buffer(30);

	buffer.setMirroredPixel(4, RGBu(10, 20, 30));

	EXPECT_RGB_EQ(buffer[4], 10, 20, 30);
	EXPECT_RGB_EQ(buffer[25], 10, 20, 30);
}

TEST(TPixelBuffer, clear) {
	TPixelBuffer<TRGB> buffer(30);

	buffer.setMirroredPixel(4, RGBu(10, 20, 30));
	buffer.clear();

	for (int i = 0; i < 30; i++) {
		EXPECT_RGB_EQ(buffer[i], 0, 0, 0);
	}
}

TEST(TPixelBuffer, fillColor) {
	TPixelBuffer<TRGB> buffer(30);

	buffer.setMirroredPixel(4, RGBu(10, 20, 30));
	buffer.clear();

	for (int i = 0; i < 30; i++) {
		EXPECT_RGB_EQ(buffer[i], 0, 0, 0);
	}
}

TEST(TPixelBuffer, fade) {
	TPixelBuffer<TRGB, uint8_t> buffer(30);

	buffer.fillColor(RGBu(100, 80, 60));
	buffer.fade(191);

	for (int i = 0; i < 30; i++) {
		EXPECT_RGB_EQ(buffer[i], 75, 60, 45);
	}
}
