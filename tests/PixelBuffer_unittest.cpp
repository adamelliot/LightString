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

TEST(TPixelBuffer, resize) {
	TPixelBuffer<TRGB, uint8_t> buffer(30);

	EXPECT_EQ(buffer.length, 30);
	buffer.resize(40);

	buffer[35] = RGBu(100, 200, 50);

	EXPECT_EQ(buffer.length, 40);

	EXPECT_RGBu_EQ(buffer[35], 100, 200, 50);
}

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

// ------------------ Mapping Buffers ---------------------

TEST(TMappingPixelBuffer2d, creationExternalMemory) {
	RGBu rawBuff[51];

	TMappingPixelBuffer2d<TRGB> buffer(rawBuff, 10, 5);

	EXPECT_EQ(buffer.width, 10);
	EXPECT_EQ(buffer.height, 5);
	EXPECT_EQ(buffer.pixels, (rawBuff + 1));
	EXPECT_FALSE(buffer.shouldDelete);
}

TEST(TMappingPixelBuffer2d, resize) {
	TMappingPixelBuffer2d<TRGB, uint8_t> buffer(10, 20);

	EXPECT_EQ(buffer.length, 200);

	buffer[buffer.xy(5, 5)] = RGBu(100, 200, 50);
	EXPECT_RGBu_EQ(buffer[55], 100, 200, 50);

	buffer.resize(30, 30);
	EXPECT_EQ(buffer.length, 900);

	uint32_t i = buffer.xy(5, 5);

	EXPECT_EQ(i, 155);

	buffer[i] = RGBu(0, 200, 50);
	EXPECT_RGBu_EQ(buffer[155], 0, 200, 50);
}

TEST(TMappingPixelBuffer3d, creationExternalMemory) {
	RGBu rawBuff[201];

	TMappingPixelBuffer3d<TRGB> buffer(rawBuff, 20, 5, 4);

	EXPECT_EQ(buffer.width, 20);
	EXPECT_EQ(buffer.height, 5);
	EXPECT_EQ(buffer.depth, 4);
	EXPECT_EQ(buffer.pixels, (rawBuff + 1));
	EXPECT_FALSE(buffer.shouldDelete);
}



// ---------------- Blending Tests -------------------

TEST(TPixelBuffer, blendWith_RGB_RGB_u8_copy) {
	TPixelBuffer<TRGB, uint8_t> dst(5);
	TPixelBuffer<TRGBA, uint8_t> src(5);

	dst.fillColor(HTML::Black);
	src.fillColor(HTML::Red);

	dst.blendWith(src, BLEND_COPY);

	for (int i = 0; i < 5; i++) {
		EXPECT_RGBu_EQ(dst[i], 0xff, 0, 0);
	}
}

TEST(TPixelBuffer, blendWith_RGB_RGBA_u8_copy) {
	TPixelBuffer<TRGB, uint8_t> dst(5);
	TPixelBuffer<TRGBA, uint8_t> src(5);

	dst.fillColor(HTML::Black);
	src.fillColor(0x80ff0000);

	dst.blendWith(src, BLEND_COPY);

	for (int i = 0; i < 5; i++) {
		EXPECT_RGBu_EQ(dst[i], 127, 0, 0);
	}
}


TEST(TPixelBuffer, blendWith_RGB_RGB_f_copy) {
	TPixelBuffer<TRGB, float> dst(5);
	TPixelBuffer<TRGBA, float> src(5);

	dst.fillColor(HTML::Black);
	src.fillColor(HTML::Red);

	dst.blendWith(src, BLEND_COPY);

	for (int i = 0; i < 5; i++) {
		EXPECT_RGBf_EQ(dst[i], 1, 0, 0);
	}
}

TEST(TPixelBuffer, blendWith_RGB_RGBA_f_copy) {
	TPixelBuffer<TRGB, float> dst(5);
	TPixelBuffer<TRGBA, float> src(5);

	dst.fillColor(HTML::Black);
	src.fillColor(0x80ff0000);

	dst.blendWith(src, BLEND_COPY);

	for (int i = 0; i < 5; i++) {
		EXPECT_RGBf_EQ(dst[i], 0.5, 0, 0);
	}
}

TEST(TPixelBuffer, blendWith_RGB_RGB_alpha_u8_copy) {
	TPixelBuffer<TRGB, uint8_t> dst(5);
	TPixelBuffer<TRGBA, uint8_t> src(5);

	dst.fillColor(HTML::Black);
	src.fillColor(HTML::Red);

	dst.blendWith(src, BLEND_COPY, 63);

	for (int i = 0; i < 5; i++) {
		EXPECT_RGBu_EQ(dst[i], 63, 0, 0);
	}
}

TEST(TPixelBuffer, blendWith_RGB_RGBA_alpha_u8_copy) {
	TPixelBuffer<TRGB, uint8_t> dst(5);
	TPixelBuffer<TRGBA, uint8_t> src(5);

	dst.fillColor(HTML::Black);
	src.fillColor(0x80ff0000);

	dst.blendWith(src, BLEND_COPY, 63);

	for (int i = 0; i < 5; i++) {
		EXPECT_RGBu_EQ(dst[i], 31, 0, 0);
	}
}


TEST(TPixelBuffer, blendWith_RGB_RGB_alpha_f_copy) {
	TPixelBuffer<TRGB, float> dst(5);
	TPixelBuffer<TRGBA, float> src(5);

	dst.fillColor(HTML::Black);
	src.fillColor(HTML::Red);

	dst.blendWith(src, BLEND_COPY, 0.25);

	for (int i = 0; i < 5; i++) {
		EXPECT_RGBf_EQ(dst[i], 0.25, 0, 0);
	}
}

TEST(TPixelBuffer, blendWith_RGB_RGBA_alpha_f_copy) {
	TPixelBuffer<TRGB, float> dst(5);
	TPixelBuffer<TRGBA, float> src(5);

	dst.fillColor(HTML::Black);
	src.fillColor(0x80ff0000);

	dst.blendWith(src, BLEND_COPY, 0.25);

	for (int i = 0; i < 5; i++) {
		EXPECT_RGBf_EQ(dst[i], 0.125, 0, 0);
	}
}

