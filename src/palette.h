#ifndef _PALETTE_H_
#define _PALETTE_H_

#include <limits.h>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include "colortypes.h"
#include "utils.h"

#ifndef PROGMEM
#define PROGMEM
#endif

// Range for 8bit palettes
const uint16_t kPaletteSize8Bit = 256;

#define RGB_HEX(R, G, B) ((R << 16) | (G << 8) | B)
#define RGBA_HEX(R, G, B, A) (((255 - A) << 24) | (R << 16) | (G << 8) | B)

namespace LightString {

struct IPalette {
private:
public:
	virtual int getSize() = 0;
	virtual ~IPalette() {}
};

typedef uint32_t ColorPaletteData[];
typedef uint32_t ColorPaletteData1[1];
typedef uint32_t ColorPaletteData2[2];
typedef uint32_t ColorPaletteData3[3];
typedef uint32_t ColorPaletteData4[4];
typedef uint32_t ColorPaletteData5[5];
typedef uint32_t ColorPaletteData6[6];
typedef uint32_t ColorPaletteData7[7];

template <template <typename> class T, typename FORMAT = uint8_t>
struct TPalette : IPalette {

public:
	std::vector<T<FORMAT>> colorStops;
	int paletteID = INT_MIN;

	T<FORMAT> valueAt8(uint8_t index) const {
		auto size = colorStops.size();
		uint16_t indexSections = index * (size - 1);
		uint8_t section = indexSections / kPaletteSize8Bit;
		uint32_t weight = indexSections % kPaletteSize8Bit;

		T<FORMAT> col = colorStops[section];

		return col.lerp8(colorStops[(section + 1) % size], weight);
	}

	T<FORMAT> valueAt(float index) const {
		auto size = colorStops.size();
		float indexSections = index * (size - 1);
		uint8_t section = indexSections;
		float weight = indexSections - section;

		T<FORMAT> col = colorStops[section];

		return col.lerp(colorStops[(section + 1) % size], weight);
	}

	T<FORMAT> valueAtMod(float index) const {
		return valueAt(fmod(index, 1.0));
	}

	T<FORMAT> operator[] (FORMAT index) const {
		return valueAt(index);
	}

	TPalette<T, FORMAT>& operator= (const TPalette<TRGB, FORMAT> &rhs) {
		colorStops = rhs.colorStops;
		paletteID = rhs.paletteID;

		return *this;
	}

	TPalette<T, FORMAT>& operator= (const TPalette<TRGBA, FORMAT> &rhs) {
		colorStops = rhs.colorStops;
		paletteID = rhs.paletteID;

		return *this;
	}

	int getSize() { return colorStops.size(); }

	TPalette() {
		colorStops.resize(4);

		colorStops[0] = T<FORMAT>(HTML::Red);
		colorStops[1] = T<FORMAT>(HTML::Green);
		colorStops[2] = T<FORMAT>(HTML::Blue);
		colorStops[3] = T<FORMAT>(HTML::Red);
	}

	TPalette(uint8_t len, const T<FORMAT> newColors[], bool mirrored = false)
	{
		colorStops.resize(mirrored ? ((len << 1) - 1) : len);
		for (auto i = 0; i < len; i++) colorStops[i] = newColors[i];

		if (mirrored) {
			uint8_t end = colorStops.size() - 1;
			for (auto i = end; i >= len; i--) {
				colorStops[i] = colorStops[end - i];
			}
		}
	}

	TPalette(T<FORMAT> col0) {
		colorStops.push_back(col0);
	}

	TPalette(T<FORMAT> col0, T<FORMAT> col1) {
		colorStops.reserve(2);
		colorStops.push_back(col0);
		colorStops.push_back(col1);
	}

	template <typename... ARGS>
	TPalette(T<FORMAT> col0, T<FORMAT> col1, ARGS&&... args) {
		colorStops.reserve(sizeof...(ARGS) + 2);
		colorStops.push_back(col0);
		addColors(col1, args...);
	}

	template <int SIZE>
	TPalette(const uint32_t (&data)[SIZE]) {
		colorStops.resize(SIZE);
		for (uint8_t i = 0; i < colorStops.size(); i++) colorStops[i] = T<FORMAT>(data[i]);
	}

	TPalette(const uint16_t size) {
		colorStops.resize(size);
	}

	void addColors(T<FORMAT> col) {
		colorStops.push_back(col);
	}

	template <typename... ARGS>
	void addColors(T<FORMAT> col0, ARGS&&... args) {
		colorStops.reserve(sizeof...(ARGS) + 1);
		colorStops.push_back(col0);
		addColors(args...);
	}

	TPalette& scale8(uint8_t scale) {
		for (auto &color : colorStops) {
			color.scale8(scale);
		}

		return *this;
	}

	TPalette& scale(FORMAT scale) {
		for (auto &color : colorStops) {
			color *= scale;
		}

		return *this;
	}
};

/**
 * TBlendedPalette generates a common palette that can be LERPed from
 * the first to the second palette.
 */
template <template <typename> class T, typename FORMAT = uint8_t>
class TBlendedPalette : public TPalette<T, FORMAT> {
private:
	TPalette<T, FORMAT> first;
	TPalette<T, FORMAT> second;

public:
	TBlendedPalette(TPalette<T, FORMAT> &first, TPalette<T, FORMAT> &second, float ratio = 0.5)
		: first(first), second(second)
	{
		this->colorStops.resize(lcm(first.getSize() - 1, second.getSize() - 1) + 1);
		setRatio(0.5);
	}

	TPalette<T, FORMAT> &getFirst() { return first; }
	TPalette<T, FORMAT> &getSecond() { return second; }

	/**
	 * Modifies the current palette to adjust the ratio between
	 * fist and second palettes.
	 * @param ratio A range from 0 to 1. Where 0 will be the first
	 * palette and 1 will be the second.
	 * @return Returns this palette object
	 */
	TBlendedPalette<T, FORMAT> &setRatio(float ratio) {
		for (int i = 0; i < this->getSize(); i++) {
			float step = (float)i / (float)(this->getSize() - 1);
			this->colorStops[i] = first.valueAt(step);
			this->colorStops[i].lerp(second.valueAt(step), ratio);
		}

		return *this;
	}
};

/* ----------------- Specializations ----------------- */

template <>
inline TRGB<uint8_t> TPalette<TRGB, uint8_t>::operator[] (uint8_t index) const {
	return valueAt8(index);
};

template <>
inline TRGBA<uint8_t> TPalette<TRGBA, uint8_t>::operator[] (uint8_t index) const {
	return valueAt8(index);
};

template <>
inline TRGB<float> TPalette<TRGB, float>::operator[] (float index) const {
	return valueAt(index);
};

template <>
inline TRGBA<float> TPalette<TRGBA, float>::operator[] (float index) const {
	return valueAt(index);
};

/* --------------- Palette Definitions --------------- */ 

typedef TPalette<TRGB, uint8_t> CRGBPalette;
typedef TPalette<TRGB, uint8_t> RGBuPalette;
typedef TPalette<TRGB, uint8_t> RGBAuPalette;

extern const ColorPaletteData1 PROGMEM SOLID_RED;
extern const ColorPaletteData1 PROGMEM SOLID_GREEN;
extern const ColorPaletteData1 PROGMEM SOLID_BLUE;
extern const ColorPaletteData1 PROGMEM SOLID_AQUA;
extern const ColorPaletteData1 PROGMEM SOLID_AZURE;
extern const ColorPaletteData7 PROGMEM HSV_GRADIENT;
extern const ColorPaletteData4 PROGMEM RAINBOW_GRADIENT;
extern const ColorPaletteData3 PROGMEM BLUE_GREEN_GRADIENT;
extern const ColorPaletteData3 PROGMEM GREEN_GRADIENT;
extern const ColorPaletteData3 PROGMEM LIME_GRADIENT;
extern const ColorPaletteData3 PROGMEM YELLOW_GRADIENT;
extern const ColorPaletteData3 PROGMEM WHITE_GRADIENT;
extern const ColorPaletteData2 PROGMEM RED_GREEN_GRADIENT;
extern const ColorPaletteData3 PROGMEM GREEN_BLUE_GRADIENT;
extern const ColorPaletteData5 PROGMEM RED_ORGANGE_GRADIENT;
extern const ColorPaletteData3 PROGMEM BLUE_WHITE_GRADIENT;
extern const ColorPaletteData3 PROGMEM BLUE_WHITISH_GRADIENT;
extern const ColorPaletteData4 PROGMEM BLUE_WHITE_YELLOW_GRADIENT;
extern const ColorPaletteData5 PROGMEM BLUE_YELLOW_BLACK_GRADIENT;
extern const ColorPaletteData5 PROGMEM RED_WHITE_BLACK_GRADIENT;
extern const ColorPaletteData4 PROGMEM RED_WHITE_GRADIENT;
extern const ColorPaletteData4 PROGMEM GREEN_WHITE_GRADIENT;
extern const ColorPaletteData4 PROGMEM RED_WHITISH_GRADIENT;
extern const ColorPaletteData3 PROGMEM CYAN_PINK_GRADIENT;
extern const ColorPaletteData3 PROGMEM BLUE_YELLOW_GRADIENT;
extern const ColorPaletteData3 PROGMEM GREEN_YELLOW_GRADIENT;
extern const ColorPaletteData5 PROGMEM RAINBOW_BLACK_GRADIENT;
extern const ColorPaletteData3 PROGMEM PINK_PURPLE_GRADIENT;
extern const ColorPaletteData5 PROGMEM PINK_PURPLE_BLACK_GRADIENT;
extern const ColorPaletteData5 PROGMEM CYAN_PINK_BLACK_GRADIENT;
extern const ColorPaletteData5 PROGMEM GREEN_BLUE_BLACK_GRADIENT;
extern const ColorPaletteData5 PROGMEM RED_ORGANGE_BLACK_GRADIENT;
extern const ColorPaletteData5 PROGMEM BLUE_WHITE_BLACK_GRADIENT;
extern const ColorPaletteData4 PROGMEM BLUE_YELLOW_BLACK_HARD_GRADIENT;
extern const ColorPaletteData2 PROGMEM WHITE_SOLID_GRADIENT;

/* --------------- Swatch Manager --------------- */ 

template<template <typename> class T, typename FORMAT = uint8_t>
class TSwatchManager {
protected:
	std::vector<T<FORMAT>> swatches;
	uint8_t swatchIndex = 0;

public:

	TSwatchManager() {}

	void shuffle();
	uint8_t getSwatchCount() { return swatches.size(); }

	T<FORMAT> getColor() { return swatches[swatchIndex]; }
	TPalette<T, FORMAT> generatePalette(uint8_t colorStops, uint8_t padding = 1, bool mirrored = true);
	bool add(T<FORMAT> color);

	T<FORMAT> next();
	T<FORMAT> previous();
};

/* --------------- Palette Manager --------------- */ 

// Manages a reference list of palettes
template<template <typename> class T, typename FORMAT = uint8_t>
class TPaletteManager {
protected:
	std::vector<TPalette<T, FORMAT>> palettes;
	size_t paletteIndex = 0;

public:

	TPaletteManager() {}

	void shuffle();
	uint8_t getPaletteCount() { return palettes.size(); }

	// Find the first palette with a specific ID
	int getPaletteIndexFromID(int id);

	T<FORMAT> getColor(FORMAT index) { return (palettes[paletteIndex])[index]; }
	inline TPalette<T, FORMAT> &getPalette() { return palettes[paletteIndex]; }
	inline TPalette<T, FORMAT> &getPalette(size_t index) { return palettes[index]; }
	inline TPalette<T, FORMAT> *getPaletteByID(int id);

	void loadPalette(size_t index) { paletteIndex = index; }
	void loadPaletteByID(uint8_t id);

	void add(const TPalette<T, FORMAT> &palette) { palettes.push_back(palette); }
	void add(int id, const TPalette<T, FORMAT> &palette);

	void next();
	void previous();
};

#include "palette.tpp"

};

using namespace LightString;

extern TSwatchManager<TRGBA, uint8_t> Swatches;
extern TPaletteManager<TRGBA, uint8_t> Palettes;

#endif
