#pragma once

#include <utility>
using namespace std;

#ifndef ARDUINO
#include <stdio.h>
#endif

namespace LightString {

template <typename TYPE> struct THSV;
template <typename TYPE> struct TRGBA;

template <typename TYPE>
struct TRGB {
	union {
		struct {
			union {
				TYPE r;
				TYPE red;
			};
			union {
				TYPE g;
				TYPE green;
			};
			union {
				TYPE b;
				TYPE blue;
			};
		};
		TYPE raw[3];
	};

	/* ------------- Constructors ------------ */

	inline TRGB() __attribute__((always_inline)) : r(0), g(0), b(0) {}

	inline TRGB(TYPE r, TYPE g, TYPE b) __attribute__((always_inline)) {
		this->r = r;
		this->g = g;
		this->b = b;
	}

	inline TRGB(uint32_t colorcode) __attribute__((always_inline)) {
		this->r = (colorcode >> 16) & 0xff;
		this->g = (colorcode >>  8) & 0xff;
		this->b = (colorcode >>  0) & 0xff;
	}

	inline TRGB(const TRGB<TYPE> &rhs) __attribute__((always_inline)) {
		this->r = rhs.r;
		this->g = rhs.g;
		this->b = rhs.b;
	}

	inline TRGB(const THSV<TYPE> &rhs) __attribute__((always_inline));
	inline TRGB(const TRGBA<TYPE> &rhs) __attribute__((always_inline));

	inline TRGB(const CRGB &rhs) __attribute__((always_inline)) {
		this->r = rhs.r;
		this->g = rhs.g;
		this->b = rhs.b;
	}

	/* --------------- Casting ----------------- */

	inline operator bool() const __attribute__((always_inline)) {
		return r || g || b;
	}

	inline THSV<TYPE> toHSV() const;

	/* -------------- Operators ---------------- */

	inline TRGB& operator+= (const TRGB<TYPE> &rhs) __attribute__((always_inline)) {
		qadd8(this->raw, rhs.raw, 3);
		return *this;
	}

	inline TRGB& operator+= (const TYPE val) __attribute__((always_inline)) {
		qadd8(this->raw, val, 3);
		return *this;
	}

	inline TRGB& operator++ () __attribute__((always_inline)) {
		operator+=(1);
		return *this;
	}

	inline TRGB operator++ (int) __attribute__((always_inline)) {
		TRGB tmp(*this);
		operator++();
		return tmp;
	}

	inline TRGB& operator-= (const TRGB<TYPE> &rhs) __attribute__((always_inline)) {
		qsub8(this->raw, rhs.raw, 3);
		return *this;
	}

	inline TRGB& operator-= (const TYPE val) __attribute__((always_inline)) {
		qsub8(this->raw, val, 3);
		return *this;
	}

	inline TRGB& operator-- () __attribute__((always_inline)) {
		operator-=(1);
		return *this;
	}

	inline TRGB operator-- (int) __attribute__((always_inline)) {
		TRGB tmp(*this);
		operator--();
		return tmp;
	}

	inline TRGB& operator*= (const TYPE val) __attribute__((always_inline)) {
		qmul8(this->raw, val, 3);
		return *this;
	}

	inline TRGB operator* (const TYPE val) __attribute__((always_inline)) {
		TRGB<TYPE> ret = *this;
		qmul8(ret.raw, val, 3);
		return ret;
	}

	inline TRGB& operator/= (const TYPE val) __attribute__((always_inline)) {
		div<TYPE>(this->raw, val, 3);
		return *this;
	}

	inline TRGB& operator%= (const uint8_t ratio) __attribute__((always_inline)) {
		::scale8(this->raw, ratio, 3);
		return *this;
	}

	/* ----------- Color Manipulation ------------ */

	inline TRGB &lerp(const TRGB<TYPE> &other, const float ratio) __attribute__((always_inline)) {
		if (ratio >= 1) {
			return *this = other;
		}
		if (ratio <= 0) {
			return *this;
		}

		uint8_t r8 = ratio * 256;
		lerp8by8(this->raw, other.raw, 3, r8);
		return *this;
	}

	inline TRGB &lerp8(const TRGB<TYPE> &other, const uint8_t ratio) __attribute__((always_inline)) {
		lerp8by8(this->raw, other.raw, 3, ratio);
		return *this;
	}

	// Scales by 8 bit ratio, so 128 = 0.5, 192 = 0.75, etc.

	inline TRGB &scale8(const uint8_t ratio) {
		::scale8(this->raw, ratio, 3);
		return *this;
	}

	inline TRGB &fade(const TYPE ratio) {
		::scale8(this->raw, ratio, 3);
		return *this;
	}

	inline TRGB fadeCopy(const TYPE ratio) {
		TRGB<TYPE> ret = *this;
		::scale8(ret.raw, ratio, 3);
		return ret;
	}

	inline TYPE saturation() {
		uint8_t high = brightness();

		uint8_t low = r;
		if (g < low) low = g;
		if (b < low) low = b;

		return 255 - (((uint16_t)low * 256) / high);
	}

	inline TYPE sat() { return saturation(); }
	inline TYPE s() { return saturation(); }

	inline TYPE saturation(const TYPE val) {
		uint8_t lo = 0, mi = 1, hi = 2;

		if (raw[lo] > raw[mi]) swap(lo, mi);
		if (raw[mi] > raw[hi]) swap(mi, hi);
		if (raw[lo] > raw[mi]) swap(lo, mi);

		uint8_t newLo = (uint16_t)((255 - val) * raw[hi]) / 256;
		uint16_t ratio = ((raw[mi] - raw[lo]) * 2560) / (raw[hi] - raw[lo]);
		uint8_t newMi = ((raw[hi] - newLo) * ratio / 2560) + newLo;

		raw[lo] = newLo;
		raw[mi] = newMi;

		return val;
	}

	inline TYPE sat(const TYPE limit) { return saturation(limit); }
	inline TYPE s(const TYPE limit) { return saturation(limit); }

	inline TYPE brightness() {
		uint8_t max = r;
		if (g > max) max = g;
		if (b > max) max = b;
		return max;
	}

	inline TYPE bri() { return brightness(); }
	inline TYPE val() { return brightness(); }
	inline TYPE v() { return brightness(); }

	inline TYPE brightness(const TYPE limit) {
		uint8_t max = brightness();
		uint16_t fact = ((uint16_t)limit * 256) / max;

		r = (r * fact) / 256;
		g = (g * fact) / 256;
		b = (b * fact) / 256;

		return limit;
	}

	inline TYPE bri(const TYPE limit) { return brightness(limit); }
	inline TYPE val(const TYPE limit) { return brightness(limit); }
	inline TYPE v(const TYPE limit) { return brightness(limit); }

	inline TRGB &maximizeBrightness() {
		brightness(255);
		return *this;
	}

	/* ------------------- Other ----------------- */

#ifdef ARDUINO
	void print() {
		Serial.print("(");
		for (int i = 1; i < 3; i++) {
			Serial.print(", ");
			Serial.print(this->raw[i]);
		}
		Serial.print(")");
	}

	void println() {
		print();
		Serial.println();
	}
#else
	void print(FILE *stream = stdout) {
		fprintf(stream, "(%f, %f, %f)", this->r, this->g, this->b);
	}

	void println(FILE *stream = stdout) {
		print(stream);
		fprintf(stream, "\n");
	}
#endif
};

/* --------------- Specializations (uint8_t) ---------------*/

/* --------------- Specializations (float) ---------------*/

template <>
inline TRGB<float>::TRGB(uint32_t colorcode) {
	this->r = (float)((colorcode >> 16) & 0xff) / 0xff;
	this->g = (float)((colorcode >>  8) & 0xff) / 0xff;
	this->b = (float)((colorcode >>  0) & 0xff) / 0xff;
}

template <>
inline TRGB<float>& TRGB<float>::operator+= (const TRGB<float> &rhs) {
	add<float>(this->raw, rhs.raw, 3);
	return *this;
}

template <>
inline TRGB<float>& TRGB<float>::operator+= (const float val) {
	add<float>(this->raw, val, 3);
	return *this;
}

template <>
inline TRGB<float>& TRGB<float>::operator-= (const TRGB<float> &rhs) {
	sub<float>(this->raw, rhs.raw, 3);
	return *this;
}

template <>
inline TRGB<float>& TRGB<float>::operator-= (const float val) {
	sub<float>(this->raw, val, 3);
	return *this;
}

template <>
inline TRGB<float>& TRGB<float>::operator*= (const float val) {
	mul<float>(this->raw, val, 3);
	return *this;
}

template <>
inline TRGB<float> TRGB<float>::operator* (const float val) {
	TRGB<float> ret = *this;

	mul<float>(ret.raw, val, 3);
	return ret;
}

template <>
inline TRGB<float>& TRGB<float>::lerp(const TRGB<float> &other, const float ratio) {
	::lerp(this->raw, other.raw, 3, ratio);
	return *this;
}

template <>
inline TRGB<float>& TRGB<float>::fade(const float ratio) {
	::mul(this->raw, ratio, 3);
	return *this;
}

template <>
inline TRGB<float> TRGB<float>::fadeCopy(const float ratio) {
	TRGB<float> ret = *this;
	::mul(ret.raw, ratio, 3);
	return ret;
}

template <>
inline float TRGB<float>::brightness() {
	float max = r;
	if (g > max) max = g;
	if (b > max) max = b;
	return max;
}

template <>
inline float TRGB<float>::brightness(const float limit) {
	float max = brightness();
	float fact = limit / max;

	r *= fact;
	g *= fact;
	b *= fact;

	return limit;
}

template <>
inline TRGB<float>& TRGB<float>::maximizeBrightness() {
	brightness(1);
	return *this;
}

template <>
inline float TRGB<float>::saturation() {
	float high = brightness();

	float low = r;
	if (g < low) low = g;
	if (b < low) low = b;

	return 1.0f - (low / high);
}

template <>
inline float TRGB<float>::saturation(const float val) {
	uint8_t lo = 0, mi = 1, hi = 2;

	if (raw[lo] > raw[mi]) swap(lo, mi);
	if (raw[mi] > raw[hi]) swap(mi, hi);
	if (raw[lo] > raw[mi]) swap(lo, mi);

	float newLo = (1 - val) * raw[hi];
	float ratio = (raw[mi] - raw[lo]) / (raw[hi] - raw[lo]);
	float newMi = (raw[hi] - newLo) * ratio + newLo;

	raw[lo] = newLo;
	raw[mi] = newMi;

	return val;
}

};
