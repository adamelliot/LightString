#ifndef _PALETTE_H_
#define _PALETTE_H_

#include <stdint.h>
#include <stdio.h>
#include "colortypes.h"

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
	virtual uint16_t getColorTotalStops() = 0;
};

typedef uint32_t ColorPaletteData[];

template <template <typename> class T, typename FORMAT = uint8_t>
struct TPalette : IPalette {

protected:
	inline void alloc() { colorStops = new T<FORMAT>[size]; }

public:
	union {
		uint8_t size;
		uint8_t totalColorStops;
	};
	union {
		T<FORMAT> *colors;
		T<FORMAT> *colorStops;
	};

	TPalette(const TPalette<T, FORMAT> &rhs) : size(rhs.size) {
		alloc();

		for (int i = 0; i < rhs.size; i++) {
			this->colors[i] = rhs.colors[i];
		}
	}

	inline T<FORMAT> valueAt8(uint8_t index) {
		uint16_t indexSections = index * (size - 1);
		uint8_t section = indexSections / kPaletteSize8Bit;
		uint32_t weight = indexSections % kPaletteSize8Bit;

		T<FORMAT> col = colors[section];

		return col.lerp8(colors[(section + 1) % size], weight);
	}

	inline T<FORMAT> valueAt(float index) {
		float indexSections = index * (size - 1);
		uint8_t section = indexSections;
		float weight = indexSections - section;

		T<FORMAT> col = colors[section];

		return col.lerp(colors[(section + 1) % size], weight);
	}

	inline T<FORMAT> operator[] (FORMAT index) {
		return valueAt(index);
	}

	TPalette<T, FORMAT>& operator= (const TPalette<TRGB, FORMAT> &rhs) {
		if (size != rhs.size) {
			delete colorStops;
			size = rhs.size;
			alloc();
		}

		for (int i = 0; i < size; i++) {
			colorStops[i] = rhs.colorStops[i];
		}
		return *this;
	}

	TPalette<T, FORMAT>& operator= (const TPalette<TRGBA, FORMAT> &rhs) {
		if (size != rhs.size) {
			delete colorStops;
			size = rhs.size;
			alloc();
		}

		for (int i = 0; i < rhs.size; i++) {
			colors[i] = rhs.colors[i];
		}

		return *this;
	}

	uint16_t getColorTotalStops() { return totalColorStops; }

	inline TPalette() : size(4) {
		alloc();

		colors[0] = T<FORMAT>(HTML::Red);
		colors[1] = T<FORMAT>(HTML::Green);
		colors[2] = T<FORMAT>(HTML::Blue);
		colors[3] = T<FORMAT>(HTML::Red);
	}

	inline TPalette(uint8_t len, const T<FORMAT> newColors[], bool mirrored = false)
		: size(mirrored ? ((len << 1) - 1) : len)
	{
		memcpy(colors, newColors, len * sizeof(T<FORMAT>));

		if (mirrored) {
			uint8_t end = size - 1;
			for (uint8_t i = end; i >= len; i--) {
				colors[i] = colors[end - i];
			}
		}
	}
	
	inline TPalette(T<FORMAT> col0) : size(1)
	{ alloc(); colors[0] = col0; }

	inline TPalette(T<FORMAT> col0, T<FORMAT> col1) : size(2)
	{ alloc(); colors[0] = col0; colors[1] = col1; }

	inline TPalette(T<FORMAT> col0, T<FORMAT> col1, T<FORMAT> col2) : size(3)
	{ alloc(); colors[0] = col0; colors[1] = col1; colors[2] = col2; }

	inline TPalette(T<FORMAT> col0, T<FORMAT> col1, T<FORMAT> col2, T<FORMAT> col3) : size(4)
	{ alloc(); colors[0] = col0; colors[1] = col1; colors[2] = col2; colors[3] = col3; }

	inline TPalette(T<FORMAT> col0, T<FORMAT> col1, T<FORMAT> col2, T<FORMAT> col3, T<FORMAT> col4) : size(5)
	{ alloc(); colors[0] = col0; colors[1] = col1; colors[2] = col2; colors[3] = col3; colors[4] = col4; }
	
	inline TPalette(T<FORMAT> col0, T<FORMAT> col1, T<FORMAT> col2, T<FORMAT> col3, T<FORMAT> col4, T<FORMAT> col5) : size(6)
	{ alloc(); colors[0] = col0; colors[1] = col1; colors[2] = col2; colors[3] = col3; colors[4] = col4; colors[5] = col5; }

	inline TPalette(T<FORMAT> col0, T<FORMAT> col1, T<FORMAT> col2, T<FORMAT> col3, T<FORMAT> col4, T<FORMAT> col5, T<FORMAT> col6) : size(7)
	{ alloc(); colors[0] = col0; colors[1] = col1; colors[2] = col2; colors[3] = col3; colors[4] = col4; colors[5] = col5; colors[6] = col6; }

	template <int SIZE>
	inline TPalette(const uint32_t (&data)[SIZE]) : size(SIZE) {
		alloc();
		for (uint8_t i = 0; i < size; i++) colors[i] = T<FORMAT>(data[i]);
	}

	~TPalette() { if (size > 0) delete colorStops; }
	
	inline TPalette& scale8(uint8_t scale) {
		for (uint8_t i = 0; i < size; i++) {
			colors[i].scale8(scale);
		}

		return *this;
	}

	inline TPalette& scale(FORMAT scale) {
		for (uint8_t i = 0; i < size; i++) {
			colors[i] *= scale;
		}

		return *this;
	}
};

/* ----------------- Specializations ----------------- */

template <>
inline TRGB<uint8_t> TPalette<TRGB, uint8_t>::operator[] (uint8_t index) {
	return valueAt8(index);
};

template <>
inline TRGBA<uint8_t> TPalette<TRGBA, uint8_t>::operator[] (uint8_t index) {
	return valueAt8(index);
};

template <>
inline TRGB<float> TPalette<TRGB, float>::operator[] (float index) {
	return valueAt(index);
};

template <>
inline TRGBA<float> TPalette<TRGBA, float>::operator[] (float index) {
	return valueAt(index);
};

/* --------------- Palette Definitions --------------- */ 

typedef TPalette<TRGB, uint8_t> CRGBPalette;
typedef TPalette<TRGB, uint8_t> RGBuPalette;
typedef TPalette<TRGB, uint8_t> RGBAuPalette;

extern const ColorPaletteData PROGMEM SOLID_RED;
extern const ColorPaletteData PROGMEM SOLID_GREEN;
extern const ColorPaletteData PROGMEM SOLID_BLUE;
extern const ColorPaletteData PROGMEM SOLID_AQUA;
extern const ColorPaletteData PROGMEM SOLID_AZURE;
extern const ColorPaletteData PROGMEM HSV_GRADIENT;
extern const ColorPaletteData PROGMEM RAINBOW_GRADIENT;
extern const ColorPaletteData PROGMEM BLUE_GREEN_GRADIENT;
extern const ColorPaletteData PROGMEM GREEN_GRADIENT;
extern const ColorPaletteData PROGMEM LIME_GRADIENT;
extern const ColorPaletteData PROGMEM YELLOW_GRADIENT;
extern const ColorPaletteData PROGMEM WHITE_GRADIENT;
extern const ColorPaletteData PROGMEM RED_GREEN_GRADIENT;
extern const ColorPaletteData PROGMEM GREEN_BLUE_GRADIENT;
extern const ColorPaletteData PROGMEM RED_ORGANGE_GRADIENT;
extern const ColorPaletteData PROGMEM BLUE_WHITE_GRADIENT;
extern const ColorPaletteData PROGMEM BLUE_WHITISH_GRADIENT;
extern const ColorPaletteData PROGMEM BLUE_WHITE_YELLOW_GRADIENT;
extern const ColorPaletteData PROGMEM BLUE_YELLOW_BLACK_GRADIENT;
extern const ColorPaletteData PROGMEM RED_WHITE_BLACK_GRADIENT;
extern const ColorPaletteData PROGMEM RED_WHITE_GRADIENT;
extern const ColorPaletteData PROGMEM GREEN_WHITE_GRADIENT;
extern const ColorPaletteData PROGMEM RED_WHITISH_GRADIENT;
extern const ColorPaletteData PROGMEM CYAN_PINK_GRADIENT;
extern const ColorPaletteData PROGMEM BLUE_YELLOW_GRADIENT;
extern const ColorPaletteData PROGMEM GREEN_YELLOW_GRADIENT;
extern const ColorPaletteData PROGMEM RAINBOW_BLACK_GRADIENT;
extern const ColorPaletteData PROGMEM PINK_PURPLE_GRADIENT;
extern const ColorPaletteData PROGMEM PINK_PURPLE_BLACK_GRADIENT;
extern const ColorPaletteData PROGMEM CYAN_PINK_BLACK_GRADIENT;
extern const ColorPaletteData PROGMEM GREEN_BLUE_BLACK_GRADIENT;
extern const ColorPaletteData PROGMEM RED_ORGANGE_BLACK_GRADIENT;
extern const ColorPaletteData PROGMEM BLUE_WHITE_BLACK_GRADIENT;
extern const ColorPaletteData PROGMEM BLUE_YELLOW_BLACK_HARD_GRADIENT;
extern const ColorPaletteData PROGMEM WHITE_SOLID_GRADIENT;

/* --------------- Swatch Manager --------------- */ 

template<template <typename> class T, typename FORMAT = uint8_t>
class TSwatchManager {
protected:
	T<FORMAT> *swatches;
	uint8_t maxSwatches;
	uint8_t swatchCount;
	uint8_t swatchIndex;

public:

	TSwatchManager(uint8_t maxSwatches)
	: maxSwatches(maxSwatches), swatchCount(0), swatchIndex(0) {
		swatches = new T<FORMAT>[maxSwatches];
	}

	~TSwatchManager() { delete swatches; }

	void shuffle();
	uint8_t getSwatchCount() { return swatchCount; }

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
	TPalette<T, FORMAT> **palettes;
	uint8_t maxPalettes;
	uint8_t paletteCount;
	uint8_t paletteIndex;

public:

	TPaletteManager(uint8_t maxPalettes)
	: maxPalettes(maxPalettes), paletteCount(0), paletteIndex(0) {
		palettes = new TPalette<T, FORMAT>*[maxPalettes];
	}
	~TPaletteManager() { delete palettes; }

	void shuffle();
	uint8_t getPaletteCount() { return paletteCount; }

	T<FORMAT> getColor(FORMAT index) { return (*palettes[paletteIndex])[index]; }
	inline TPalette<T, FORMAT> getPalette() { return *palettes[paletteIndex]; }

	void loadPalette(uint8_t index) { paletteIndex = index; }

	bool add(TPalette<T, FORMAT> &palette);

	void next();
	void previous();
};

#include "palette.tpp"

};

using namespace LightString;

extern TSwatchManager<TRGBA, uint8_t> Swatches;
extern TPaletteManager<TRGBA, uint8_t> Palettes;

#endif
