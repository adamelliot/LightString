#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/Palette.h"

using namespace LightString;

TEST(TPaletteF, creation) {
	TPalette<TRGB, float> palette(HTML::Red, HTML::Lime, HTML::Blue);

	EXPECT_EQ(palette.size, 3);
	EXPECT_EQ(palette.colorStops[0].r, 1.0);
	EXPECT_EQ(palette.colorStops[1].g, 1.0);
	EXPECT_EQ(palette.colorStops[2].b, 1.0);
}

TEST(TPaletteF, indexAccess) {
	TPalette<TRGB, float> palette1(HTML::Red, HTML::Lime);

	EXPECT_RGBf_EQ(palette1[0.5], 0.5, 0.5, 0);

	TPalette<TRGBA, float> palette2(HTML::Red, HTML::Lime);

	EXPECT_RGBAf_EQ(palette2[0.5], 0.5, 0.5, 0, 1);

}

TEST(TPaletteF, scale) {
	TPalette<TRGB, float> palette(HTML::Red, HTML::Lime, HTML::Blue);

	palette.scale(0.5);

	EXPECT_RGBf_EQ(palette.colorStops[1], 0, 0.5, 0);
}

// ------------------- Swatch Manager ---------------------

TEST(TSwatchManagerF, creation) {
	TSwatchManager<TRGB, float> swatches(10);

	EXPECT_EQ(swatches.getSwatchCount(), 0);
}

TEST(TSwatchManagerF, add) {
	TSwatchManager<TRGB, float> swatches(10);

	swatches.add(HTML::Red);

	EXPECT_EQ(swatches.getSwatchCount(), 1);
	EXPECT_RGBf_EQ(swatches.next(), 1, 0, 0);
}

TEST(TSwatchManagerF, next) {
	TSwatchManager<TRGB, float> swatches(10);

	swatches.add(HTML::Red);
	swatches.add(HTML::Lime);

	RGBf col1 = swatches.next();
	RGBf col2 = swatches.next();

	EXPECT_EQ(swatches.getSwatchCount(), 2);
	EXPECT_RGBf_EQ(col1, 0, 1, 0)
	EXPECT_RGBf_EQ(col2, 1, 0, 0)
}

TEST(TSwatchManagerF, getColor) {
	TSwatchManager<TRGB, float> swatches(10);

	swatches.add(HTML::Red);
	swatches.add(HTML::Lime);
	swatches.add(HTML::Blue);

	RGBf col1 = swatches.next();
	RGBf col2 = swatches.getColor();

	EXPECT_EQ(swatches.getSwatchCount(), 3);
	EXPECT_RGBf_EQ(col1, 0, 1, 0);
	EXPECT_RGBf_EQ(col2, 0, 1, 0);
}

// ------------------- Palette Manager ---------------------

TEST(TPaletteManagerF, creation) {
	TPaletteManager<TRGB, float> palettes(10);

	EXPECT_EQ(palettes.getPaletteCount(), 0);
}

TEST(TPaletteManagerF, add) {
	TPaletteManager<TRGB, float> palettes(10);
	TPalette<TRGB, float> pal(HTML::Red, HTML::Lime);

	palettes.add(pal);

	EXPECT_EQ(palettes.getPaletteCount(), 1);
}

TEST(TPaletteManagerF, getColor) {
	TPaletteManager<TRGB, float> palettes(10);
	TPalette<TRGB, float> pal(HTML::Red, HTML::Lime);

	palettes.add(pal);

	RGBf col = palettes.getColor(0);
	EXPECT_RGBf_EQ(col, 1, 0, 0);

	col = palettes.getColor(1);
	EXPECT_RGBf_EQ(col, 0, 1, 0);
}

TEST(TPaletteManagerF, next) {
	TPaletteManager<TRGB, float> palettes(10);

	TPalette<TRGB, float> pal1(HTML::Blue, HTML::Yellow);
	TPalette<TRGB, float> pal2(HTML::Red, HTML::Lime);

	palettes.add(pal1);
	palettes.add(pal2);

	palettes.next();

	RGBf col1 = palettes.getColor(0);
	RGBf col2 = palettes.getColor(1);

	EXPECT_RGBf_EQ(col1, 1, 0, 0);
	EXPECT_RGBf_EQ(col2, 0, 1, 0);
}
