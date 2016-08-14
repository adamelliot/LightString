#pragma once

#include <string.h>
#include <math.h>
#include <algorithm>
#include <typeindex>

#include "utils.h"
#include "types.h"
#include "geometry.h"
#include "colortypes.h"

namespace LightString {

template <template <typename> class T, typename FORMAT = uint8_t>
class TPixelBuffer : public IPixelBuffer {
public:
	T<FORMAT> *pixels;

	uint16_t length;
	bool shouldDelete;

	inline TPixelBuffer()
		: pixels(0), length(0), shouldDelete(false) {}

	inline TPixelBuffer(T<FORMAT> *pixels, uint16_t length)
		: pixels(pixels), length(length), shouldDelete(false) {}

	inline TPixelBuffer(const uint16_t length) : length(length) {
		pixels = new T<FORMAT>[length];
		memset(pixels, 0, sizeof(T<FORMAT>) * length);
		shouldDelete = true;
	}

	inline ~TPixelBuffer() {
		if (shouldDelete) delete pixels;
	}

	inline bool resize(uint16_t length) {
		if (!shouldDelete && length > 0) {
#ifdef ARDUINO
			Serial.println("ERROR: Cannot resize buffer that is not owned by pixel buffer.");
#else
			fprintf(stderr, "ERROR: Cannot resize buffer that is not owned by pixel buffer.\n");
#endif
			return false;
		}

		if (shouldDelete) {
			free(pixels);
		}

		this->length = length;
		pixels = (T<FORMAT> *) malloc(sizeof(T<FORMAT>) * length);
		memset(pixels, 0, sizeof(T<FORMAT>) * length);

		return true;
	}

	inline uint16_t getLength() const {
		return length;
	}

	inline T<FORMAT>& operator[] (uint16_t index) __attribute__((always_inline)) {
		return pixels[index];
	}

	inline void setPixel(uint16_t index, T<FORMAT> col) __attribute__((always_inline)) {
		pixels[index] = col;
	}

	// FIXME: Alpha channel semantics should be reviewed
	inline void setPixelAA(float index, T<FORMAT> col) __attribute__((always_inline)) {
		uint16_t base = (uint16_t)index;
		uint8_t ratio = (uint8_t)(fmod(index, 1) * 255);
		if (ratio == 0) {
			setPixel(base, col);
			return;
		}

		pixels[base] = pixels[base].lerp8(col, 255 - ratio);
		if ((base + 1) < length) {
			pixels[base + 1] = pixels[base + 1].lerp8(col, ratio);
		}
	}

	inline void setPixels(uint16_t index, uint8_t length, T<FORMAT> col) __attribute__((always_inline)) {
		for (uint16_t i = index; i < index + length; i++) {
			pixels[i] = col;
		}
	}

	inline void setMirroredPixel(uint16_t index, T<FORMAT> col) __attribute__((always_inline)) {
		pixels[index] = col;
		pixels[length - index - 1] = col;
	}

	inline void clear() __attribute__((always_inline)) {
		memset(pixels, 0, sizeof(T<FORMAT>) * length);
	}

	inline void fillColor(T<FORMAT> col) __attribute__((always_inline)) {
		for ( int i = 0; i < length; i++) {
			pixels[i] = col;
		}
	}

	inline void fade(const FORMAT scale) {
		return;
	}

	/*
		// 2d stuff should be broken out into 2d buffer
		inline void drawRect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, CRGB col) {
			::drawRect(pixels, x0, y0, x1, y1, col);
		}

		inline void drawRect(Point<float> pt, uint8_t size, CRGB col) {
			::drawRect(pixels, pt.x, pt.y, pt.x + size, pt.y + size, col);
		}

		inline void lineTo(Point<float> pt1, Point<float> pt2, CRGB col) {
			::lineTo(pixels, pt1.x, pt1.y, pt2.x, pt2.y, col);
		}

		inline void drawSolidCircle(int16_t x, int16_t y, uint8_t radius, CRGB col) {
			::drawSolidCircle(pixels, x, y, radius, col);
		}

		inline void drawSolidCircle(Point<float> pt, uint8_t radius, CRGB col) {
			::drawSolidCircle(pixels, pt.x, pt.y, radius, col);
		}
	*/

	template <template <typename> class SRC_PIXEL>
	inline TPixelBuffer<T, FORMAT> &blendCOPY(TPixelBuffer<SRC_PIXEL, FORMAT> &src) {
		uint16_t len = std::min(this->length, src.length);
		for (uint16_t i = 0; i < len; i++) {
			// BUG: FP Blending isn't fully working with copying
			// LightString::blendCOPY(this->pixels[i], src.pixels[i]);
			this->pixels[i] = src.pixels[i];
		}

		return *this;
	}

	template <template <typename> class SRC_PIXEL>
	inline TPixelBuffer<T, FORMAT> &blendADD(TPixelBuffer<SRC_PIXEL, FORMAT> &src) {
		uint16_t len = std::min(this->length, src.length);
		for (uint16_t i = 0; i < len; i++) {
			LightString::blendADD(this->pixels[i], src.pixels[i]);
		}

		return *this;
	}

	template <template <typename> class SRC_PIXEL>
	inline TPixelBuffer<T, FORMAT> &blendWith(TPixelBuffer<SRC_PIXEL, FORMAT> &src, EBlendMode blendMode) {
		switch (blendMode) {
		case BLEND_COPY: return blendCOPY(src);
		case BLEND_ADD: return blendADD(src);
		}

		return *this;
	}

	inline TPixelBuffer<T, FORMAT> &blendWith(IPixelBuffer &src, EBlendMode blendMode) {
		static TPixelBuffer<TRGB, FORMAT> rgbType;
		static TPixelBuffer<TRGBA, FORMAT> rgbaType;

		// TODO: This method is heavy and needs to be rethought, dynamic_cast is heavy and brittle

		TPixelBuffer<TRGB, FORMAT> *rgb = dynamic_cast<TPixelBuffer<TRGB, FORMAT> *>(&src);
		if (rgb) {
			return blendWith(*rgb, blendMode);
		}

		TPixelBuffer<TRGBA, FORMAT> *rgba = dynamic_cast<TPixelBuffer<TRGBA, FORMAT> *>(&src);
		if (rgba) {
			return blendWith(*rgba, blendMode);
		}

#ifdef ARDUINO
		Serial.println("ERROR: blendWith didn't find a suitable [src] type. Blending failed");
#else
		fprintf(stderr, "blendWith didn't find a suitable [src] type. Blending failed\n");
#endif

		return *this;
	}
};

template <>
inline void TPixelBuffer<TRGB, uint8_t>::fade(const uint8_t scale) {
	for (uint16_t i = 0; i < this->length; i++) {
		pixels[i].scale8(scale);
	}
}

template <>
inline void TPixelBuffer<TRGBA, uint8_t>::fade(const uint8_t scale) {
	for (uint16_t i = 0; i < this->length; i++) {
		pixels[i].scale8(scale);
	}
}
/*
template <>
inline void TPixelBuffer<TRGB, float>::fade(const float scale) {
	for (uint16_t i = 0; i < this->length; i++) {
		pixels[i] *= scale
	}
}

template <>
inline void TPixelBuffer<TRGBA, float>::fade(const float scale) {
	for (uint16_t i = 0; i < this->length; i++) {
		pixels[i] *= scale;
	}
}
*/
typedef TPixelBuffer<TRGB, uint8_t> RGBuBuffer;
typedef TPixelBuffer<TRGB, uint8_t> RGBBuffer;
typedef TPixelBuffer<TRGBA, uint8_t> RGBABuffer;
typedef TPixelBuffer<TRGBA, uint8_t> RGBAuBuffer;

};

#include "buffers/MappingPixelBuffer2d.h"
#include "buffers/MappingPixelBuffer3d.h"
