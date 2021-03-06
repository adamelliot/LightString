#pragma once

#ifndef ARDUINO
#include <stdio.h>
#endif

#include "RGB.h"

namespace LightString {

template <typename TYPE>
struct TRGBA : TRGB<TYPE> {
	union {
		TYPE a;
		TYPE alpha;
	};

	/* ------------- Constructors ------------ */

	inline TRGBA() __attribute__((always_inline)) : TRGB<TYPE>(0, 0, 0) {
		this->a = 255;
	}

	inline TRGBA(const THSV<TYPE> &rhs) __attribute__((always_inline));

	inline TRGBA(const TRGB<TYPE> &rhs) {
		this->r = rhs.r;
		this->g = rhs.g;
		this->b = rhs.b;
		this->a = 0xff;
	}

	inline TRGBA(TYPE r, TYPE g, TYPE b) __attribute__((always_inline)) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = 0xff;
	}

	inline TRGBA(TYPE r, TYPE g, TYPE b, TYPE a) __attribute__((always_inline)) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	inline TRGBA(uint32_t colorcode) __attribute__((always_inline)) {
		this->r = (colorcode >> 16) & 0xff;
		this->g = (colorcode >>  8) & 0xff;
		this->b = (colorcode >>  0) & 0xff;
		// Invert high values so existing color codes work, but alpha ones can be added
		// kinda gross, but gets the job done.
		this->a = 0xff - ((colorcode >> 24) & 0xff);
	}

	/* --------------- Casting ----------------- */

	inline operator bool() const __attribute__((always_inline)) {
		return this->r || this->g || this->b || a;
	}

	/* -------------- Operators ---------------- */

	inline TRGBA& operator+= (const TRGB<TYPE> &rhs) __attribute__((always_inline)) {
		qadd8(this->raw, rhs.raw, 3);
		return *this;
	}

	inline TRGBA& operator+= (const TYPE val) __attribute__((always_inline)) {
		qadd8(this->raw, val, 3);
		return *this;
	}

	inline TRGBA& operator++ () __attribute__((always_inline)) {
		operator+=(1);
		return *this;
	}

	inline TRGBA operator++ (int) __attribute__((always_inline)) {
		TRGBA tmp(*this);
		operator++();
		return tmp;
	}

	inline TRGBA& operator-= (const TRGB<TYPE> &rhs) __attribute__((always_inline)) {
		qsub8(this->raw, rhs.raw, 3);
		return *this;
	}

	inline TRGBA& operator-= (const TYPE val) __attribute__((always_inline)) {
		qsub8(this->raw, val, 3);
		return *this;
	}

	inline TRGBA& operator-- () __attribute__((always_inline)) {
		operator-=(1);
		return *this;
	}

	inline TRGBA operator-- (int) __attribute__((always_inline)) {
		TRGBA tmp(*this);
		operator--();
		return tmp;
	}

	inline TRGBA& operator*= (const TYPE val) __attribute__((always_inline)) {
		qmul8(this->raw, val, 3);
		return *this;
	}

	inline TRGBA operator* (const TYPE val) __attribute__((always_inline)) {
		TRGBA<TYPE> ret = *this;
		qmul8(ret.raw, val, 3);
		return ret;
	}

	inline TRGBA& operator/= (const TYPE val) __attribute__((always_inline)) {
		div<TYPE>(this->raw, val, 3);
		return *this;
	}

	inline TRGBA& operator%= (const uint8_t ratio) __attribute__((always_inline)) {
		::scale8(this->raw, ratio, 3);
		return *this;
	}

	inline TRGBA& operator&= (const TRGB<TYPE> &rhs) {
		blendCOPY(*this, rhs);
		return *this;
	}

	inline TRGBA& operator&= (const TRGBA<TYPE> &rhs) {
		blendCOPY(*this, rhs);
		return *this;
	}

	inline TRGBA& operator|= (const TRGB<TYPE> &rhs) {
		blendADD(*this, rhs);
		return *this;
	}

	inline TRGBA& operator|= (const TRGBA<TYPE> &rhs) {
		blendADD(*this, rhs);
		return *this;
	}

	/* ----------- General Utilities ------------ */

	inline TRGBA &lerp(const TRGB<TYPE> &other, float ratio) __attribute__((always_inline)) {
		uint8_t r8 = ratio * 0xff;
		lerp8by8(this->raw, other.raw, 3, r8);
		return *this;
	}

	inline TRGBA &lerp(const TRGBA<TYPE> &other, float ratio) __attribute__((always_inline)) {
		uint8_t r8 = ratio * 0xff;
		lerp8by8(this->raw, other.raw, 4, r8);
		return *this;
	}

	inline TRGBA &lerp8(const TRGB<TYPE> &other, uint8_t ratio) __attribute__((always_inline)) {
		lerp8by8(this->raw, other.raw, 3, ratio);
		return *this;
	}

	inline TRGBA &lerp8(const TRGBA<TYPE> &other, uint8_t ratio) __attribute__((always_inline)) {
		lerp8by8(this->raw, other.raw, 4, ratio);
		return *this;
	}

	inline TRGBA &scale8(const uint8_t ratio) {
		::scale8(this->raw, ratio, 3);
		return *this;
	}

	inline TRGBA &fade(const TYPE ratio) {
		::scale8(this->raw, ratio, 3);
		return *this;
	}

	inline TRGBA fadeCopy(const TYPE ratio) {
		auto ret = *this;
		::scale8(ret.raw, ratio, 3);
		return ret;
	}

	inline TRGBA &alphaFade(const TYPE ratio) {
		this->a = ::scale8(this->a, ratio);
		return *this;
	}

	inline TRGBA alphaFadeCopy(const TYPE ratio) {
		TRGBA<TYPE> ret = *this;
		return ret.alphaFade(ratio);
	}

	/* ------------------- Other ----------------- */

#ifdef ARDUINO
	void print() {
		Serial.print("(");
		Serial.print(0);
		for (int i = 1; i < 4; i++) {
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
		fprintf(stream, "(%f, %f, %f, %f)", this->r, this->g, this->b, this->a);
	}

	void println(FILE *stream = stdout) {
		print(stream);
		fprintf(stream, "\n");
	}
#endif
};

/* --------------- Specializations ---------------*/ 

template <typename TYPE>
inline TRGB<TYPE>::TRGB(const TRGBA<TYPE> &rhs) {
	this->r = rhs.r;
	this->g = rhs.g;
	this->b = rhs.b;
}

/* --------------- Specializations (float) ---------------*/ 

template <>
inline TRGBA<float>::TRGBA() : TRGB<float>(0, 0, 0) {
	this->a = 1;
}

template <>
inline TRGBA<float>::TRGBA(const TRGB<float> &rhs) {
	this->r = rhs.r;
	this->g = rhs.g;
	this->b = rhs.b;
	this->a = 1;
}

template <>
inline TRGBA<float>::TRGBA(float r, float g, float b) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = 1;
}

template <>
inline TRGBA<float>::TRGBA(float r, float g, float b, float a) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

template <>
inline TRGBA<float>::TRGBA(uint32_t colorcode) {
	this->r = (float)((colorcode >> 16) & 0xff) / 255.0f;
	this->g = (float)((colorcode >>  8) & 0xff) / 255.0f;
	this->b = (float)((colorcode >>  0) & 0xff) / 255.0f;
	this->a = (float)(0xff - ((colorcode >> 24) & 0xff)) / 255.0f;
}

template <>
inline TRGBA<float>& TRGBA<float>::operator+= (const TRGB<float> &rhs) {
	add<float>(this->raw, rhs.raw, 3);
	return *this;
}

template <>
inline TRGBA<float>& TRGBA<float>::operator+= (const float val) {
	add<float>(this->raw, val, 3);
	return *this;
}

template <>
inline TRGBA<float>& TRGBA<float>::operator-= (const TRGB<float> &rhs) {
	sub<float>(this->raw, rhs.raw, 3);
	return *this;
}

template <>
inline TRGBA<float>& TRGBA<float>::operator-= (const float val) {
	sub<float>(this->raw, val, 3);
	return *this;
}

template <>
inline TRGBA<float>& TRGBA<float>::operator*= (const float val) {
	mul<float>(this->raw, val, 3);
	return *this;
}

template <>
inline TRGBA<float> TRGBA<float>::operator* (const float val) {
	TRGBA<float> ret = *this;
	mul<float>(ret.raw, val, 3);
	return ret;
}

template <>
inline TRGBA<float> &TRGBA<float>::lerp(const TRGB<float> &other, float ratio) {
	::lerp(this->raw, other.raw, 3, ratio);
	return *this;
}

template <>
inline TRGBA<float> &TRGBA<float>::lerp(const TRGBA<float> &other, float ratio) {
	::lerp(this->raw, other.raw, 4, ratio);
	return *this;
}

template <>
inline TRGBA<float> &TRGBA<float>::fade(const float ratio) {
	::mul(this->raw, ratio, 3);
	return *this;
}

template <>
inline TRGBA<float> TRGBA<float>::fadeCopy(const float ratio) {
	TRGBA<float> ret = *this;
	::mul(ret.raw, ratio, 3);
	return ret;
}

template <>
inline TRGBA<float> &TRGBA<float>::alphaFade(const float ratio) {
	this->a *= ratio;
	return *this;
}

template <>
inline TRGBA<float> TRGBA<float>::alphaFadeCopy(const float ratio) {
	TRGBA<float> ret = *this;
	ret.a *= ratio;
	return ret;
}

/* --------------- Blending ---------------*/

template <typename TYPE>
TRGBA<TYPE> blendCOPY(TRGBA<TYPE> &lhs, const TRGBA<TYPE> &rhs);

template <typename TYPE>
TRGB<TYPE> blendCOPY(TRGB<TYPE> &lhs, const TRGBA<TYPE> &rhs);

template <typename TYPE>
TRGB<TYPE> blendCOPY(TRGB<TYPE> &lhs, const TRGB<TYPE> &rhs);

template <>
inline TRGBA<uint8_t> blendCOPY(TRGBA<uint8_t> &lhs, const TRGBA<uint8_t> &rhs) {
	// TODO: Reimplement using proper alpha compositing (http://en.wikipedia.org/wiki/Alpha_compositing)
	uint8_t a = rhs.a;
	lhs.scale8(lhs.a);
	lhs.lerp8(rhs, rhs.a);
	lhs.a = a;
	return lhs;
}

template <>
inline TRGBA<float> blendCOPY(TRGBA<float> &lhs, const TRGBA<float> &rhs) {
	for (auto i = 0; i < 3; i++)
		lhs.raw[i] = (rhs.raw[i] * rhs.a) + (lhs.raw[i] * lhs.a * (1 - rhs.a));

	lhs.a = lhs.a + (1 - lhs.a) * rhs.a;

	return lhs;
}

template <>
inline TRGB<uint8_t> blendCOPY(TRGB<uint8_t> &lhs, const TRGBA<uint8_t> &rhs) {
	lhs.lerp8(rhs, rhs.a);
	return lhs;
}

template <>
inline TRGB<float> blendCOPY(TRGB<float> &lhs, const TRGBA<float> &rhs) {
	lhs.lerp(rhs, rhs.a);
	return lhs;
}

template <>
inline TRGB<uint8_t> blendCOPY(TRGB<uint8_t> &lhs, const TRGB<uint8_t> &rhs) {
	lhs = rhs;
	return lhs;
}

template <>
inline TRGB<float> blendCOPY(TRGB<float> &lhs, const TRGB<float> &rhs) {
	lhs = rhs;
	return lhs;
}

template <typename TYPE>
TRGBA<TYPE> blendADD(TRGBA<TYPE> &lhs, const TRGBA<TYPE> &rhs);

template <typename TYPE>
inline TRGB<TYPE> blendADD(TRGB<TYPE> &lhs, const TRGBA<TYPE> &rhs) {
	auto tmp = rhs;
	tmp *= rhs.a;
	lhs += rhs;

	return lhs;
}

template <typename TYPE>
inline TRGB<TYPE> blendADD(TRGB<TYPE> &lhs, const TRGB<TYPE> &rhs) {
	lhs += rhs;
	return lhs;
}

template <>
inline TRGBA<uint8_t> blendADD(TRGBA<uint8_t> &lhs, const TRGBA<uint8_t> &rhs) {
	TRGBA<uint8_t> adj = TRGBA<uint8_t>(rhs).scale8(rhs.a);
	lhs.scale8(lhs.a);
	lhs += adj;
	return lhs;
}

template <>
inline TRGBA<float> blendADD(TRGBA<float> &lhs, const TRGBA<float> &rhs) {
	TRGB<float> adj = TRGB<float>(rhs) * rhs.a;
	lhs *= lhs.a;
	lhs += adj;
	return lhs;
}

};

