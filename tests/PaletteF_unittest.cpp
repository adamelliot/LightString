#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/palette.h"

using namespace LightString;

TEST(TPaletteF, creation) {
	TPalette<TRGB, float> palette(HTML::Red, HTML::Lime, HTML::Blue);

	EXPECT_EQ(palette.getSize(), 3);
	EXPECT_EQ(palette.colorStops[0].r, 1.0);
	EXPECT_EQ(palette.colorStops[1].g, 1.0);
	EXPECT_EQ(palette.colorStops[2].b, 1.0);
}

TEST(TPaletteF, creationWithLength) {
	TRGB<float> colors[] = {HTML::Red, HTML::Lime, HTML::Blue};
	TPalette<TRGB, float> palette(3, colors);

	EXPECT_EQ(palette.getSize(), 3);
	EXPECT_EQ(palette.colorStops[0].r, 1.0);
	EXPECT_EQ(palette.colorStops[1].g, 1.0);
	EXPECT_EQ(palette.colorStops[2].b, 1.0);
}

TEST(TPaletteF, creationMirroredWithLength) {
	TRGB<float> colors[] = {HTML::Red, HTML::Lime};
	TPalette<TRGB, float> palette(2, colors, true);

	EXPECT_EQ(palette.getSize(), 3);
	EXPECT_EQ(palette.colorStops[0].r, 1.0);
	EXPECT_EQ(palette.colorStops[1].g, 1.0);
	EXPECT_EQ(palette.colorStops[2].r, 1.0);
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

// ------------------- Blended Palette ---------------------

TEST(TBlendedPaletteF, creation) {
	TPalette<TRGB, float> paletteA(HTML::Red, HTML::Lime, HTML::Blue);
	TPalette<TRGB, float> paletteB(HTML::Blue, HTML::Red, HTML::Lime, HTML::White);

	TBlendedPalette<TRGB, float> palette(paletteA, paletteB);

	EXPECT_EQ(palette.getSize(), 7);

	palette.setRatio(0);
	EXPECT_RGBf_EQ(palette.colorStops[0], 1.0, 0, 0);
	EXPECT_RGBf_EQ(palette.colorStops[3], 0, 1.0, 0);
	EXPECT_RGBf_EQ(palette.colorStops[6], 0, 0, 1.0);

	EXPECT_RGBf_EQ(palette.valueAt(0.25), 0.5, 0.5, 0);
	EXPECT_RGBf_EQ(palette.valueAt(0.50), 0, 1, 0);
	EXPECT_RGBf_EQ(palette.valueAt(0.75), 0, 0.5, 0.5);
	EXPECT_RGBf_EQ(palette.valueAt(1), 0, 0, 1);

	palette.setRatio(1);
	EXPECT_RGBf_EQ(palette.colorStops[0], 0, 0, 1.0);
	EXPECT_RGBf_EQ(palette.colorStops[2], 1.0, 0, 0);
	EXPECT_RGBf_EQ(palette.colorStops[4], 0, 1.0, 0);
	EXPECT_RGBf_EQ(palette.colorStops[6], 1, 1, 1);

	EXPECT_RGBf_EQ(palette.valueAt(0.25), 0.75, 0, 0.25);
	EXPECT_RGBf_EQ(palette.valueAt(0.50), 0.5, 0.5, 0);
	EXPECT_RGBf_EQ(palette.valueAt(0.75), 0.25, 1, 0.25);
	EXPECT_RGBf_EQ(palette.valueAt(1), 1, 1, 1);

	palette.setRatio(0.25);
	EXPECT_RGBf_EQ(palette.valueAt(0.25), 0.5625, 0.375, 0.0625);
	EXPECT_RGBf_EQ(palette.valueAt(0.50), 0.125, 0.875, 0);
	EXPECT_RGBf_EQ(palette.valueAt(0.75), 0.0625, 0.625, 0.4375);
	EXPECT_RGBf_EQ(palette.valueAt(1), 0.25, 0.25, 1);
}

// ------------------- Swatch Manager ---------------------

TEST(TSwatchManagerF, creation) {
	TSwatchManager<TRGB, float> swatches;

	EXPECT_EQ(swatches.getSwatchCount(), 0);
}

TEST(TSwatchManagerF, add) {
	TSwatchManager<TRGB, float> swatches;

	swatches.add(HTML::Red);

	EXPECT_EQ(swatches.getSwatchCount(), 1);
	EXPECT_RGBf_EQ(swatches.next(), 1, 0, 0);
}

TEST(TSwatchManagerF, next) {
	TSwatchManager<TRGB, float> swatches;

	swatches.add(HTML::Red);
	swatches.add(HTML::Lime);

	RGBf col1 = swatches.next();
	RGBf col2 = swatches.next();

	EXPECT_EQ(swatches.getSwatchCount(), 2);
	EXPECT_RGBf_EQ(col1, 0, 1, 0)
	EXPECT_RGBf_EQ(col2, 1, 0, 0)
}

TEST(TSwatchManagerF, getColor) {
	TSwatchManager<TRGB, float> swatches;

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
	TPaletteManager<TRGB, float> palettes;

	EXPECT_EQ(palettes.getPaletteCount(), 0);
}

TEST(TPaletteManagerF, add) {
	TPaletteManager<TRGB, float> palettes;
	TPalette<TRGB, float> pal(HTML::Red, HTML::Lime);

	palettes.add(pal);

	EXPECT_EQ(palettes.getPaletteCount(), 1);
}

TEST(TPaletteManagerF, addWithID) {
	TPaletteManager<TRGB, float> palettes;
	TPalette<TRGB, float> pal(HTML::Red, HTML::Lime);
	TPalette<TRGB, float> pal2(HTML::Blue, HTML::Lime);

	palettes.add(2, pal);
	palettes.add(2, pal2);

	EXPECT_EQ(palettes.getPaletteCount(), 1);

	palettes.add(1, pal2);

	EXPECT_EQ(palettes.getPaletteCount(), 2);
}

TEST(TPaletteManagerF, getPaletteByID) {
	TPaletteManager<TRGB, float> palettes;
	TPalette<TRGB, float> pal(HTML::Red, HTML::Lime);
	TPalette<TRGB, float> pal2(HTML::Blue, HTML::Lime);

	palettes.add(2, pal);
	palettes.add(2, pal2);

	palettes.add(1, pal);

	auto &ret = *(palettes.getPaletteByID(2));

	EXPECT_EQ(ret.colorStops[0], pal2.colorStops[0]);
	EXPECT_EQ(ret.colorStops[1], pal2.colorStops[1]);
}

TEST(TPaletteManagerF, getPaletteIndexFromID) {
	TPaletteManager<TRGB, float> palettes;
	TPalette<TRGB, float> pal(HTML::Red, HTML::Lime);
	TPalette<TRGB, float> pal2(HTML::Blue, HTML::Lime);

	palettes.add(0, pal);
	palettes.add(0, pal2);

	palettes.add(2, pal);

	auto index = palettes.getPaletteIndexFromID(0);
	auto &ret = palettes.getPalette(index);

	EXPECT_EQ(ret.colorStops[0], pal2.colorStops[0]);
	EXPECT_EQ(ret.colorStops[1], pal2.colorStops[1]);
}

TEST(TPaletteManagerF, getColor) {
	TPaletteManager<TRGB, float> palettes;
	TPalette<TRGB, float> pal(HTML::Red, HTML::Lime);

	palettes.add(pal);

	RGBf col = palettes.getColor(0);
	EXPECT_RGBf_EQ(col, 1, 0, 0);

	col = palettes.getColor(1);
	EXPECT_RGBf_EQ(col, 0, 1, 0);
}

TEST(TPaletteManagerF, next) {
	TPaletteManager<TRGB, float> palettes;

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

TEST(TPaletteManagerF, createDirectlyFromColorData) {
	TPaletteManager<TRGB, float> palettes;

	palettes.add(SOLID_RED);
	palettes.add(HSV_GRADIENT);

	RGBf col1 = palettes.getColor(0);

	palettes.next();
	RGBf col2 = palettes.getColor(0.5);

	EXPECT_RGBf_EQ(col1, 1, 0, 0);
	EXPECT_RGBf_EQ(col2, 0, 1, 1);
}

TEST(PaletteTypesF, creation) {
	TPalette<TRGBA, float> pal = SOLID_RED;

	RGBf col = pal[0];

	EXPECT_RGBf_EQ(col, 1, 0, 0);
}

TEST(TPaletteManagerF, loadPaletteByID) {
	TPaletteManager<TRGB, float> palettes;

	TPalette<TRGB, float> pal1(HTML::Blue, HTML::Yellow);
	pal1.paletteID = 1;

	TPalette<TRGB, float> pal2(0xff0000, 0x00ff00);
	pal2.paletteID = 2;

	TPalette<TRGB, float> pal3(HTML::Pink, HTML::White);
	pal3.paletteID = 3;

	palettes.add(pal1);
	palettes.add(pal2);
	palettes.add(pal3);

	palettes.loadPaletteByID(2);

	RGBf col1 = palettes.getColor(0);
	RGBf col2 = palettes.getColor(1);

	EXPECT_RGBf_EQ(col1, 1, 0, 0);
	EXPECT_RGBf_EQ(col2, 0, 1, 0);
}
