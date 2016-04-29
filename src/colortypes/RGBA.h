#pragma once

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
		this->a = 0;
	}

	inline TRGBA(TYPE r, TYPE g, TYPE b, TYPE a = 0xff) __attribute__((always_inline)) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	/* --------------- Casting ----------------- */

	inline operator bool() const __attribute__((always_inline)) {
		return this->r || this->g || this->b || a;
	}

	/* -------------- Operators ---------------- */

	inline TRGBA& operator= (const TRGB<TYPE> &rhs) __attribute__((always_inline)) {
		this->r = rhs.r;
		this->g = rhs.g;
		this->b = rhs.b;
		
		return *this;
	}

	inline TRGBA& operator= (const TRGBA<TYPE> &rhs) __attribute__((always_inline)) {
		this->r = rhs.r;
		this->g = rhs.g;
		this->b = rhs.b;
		this->a = rhs.a;
		
		return *this;
	}

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

	inline TRGBA& operator/= (const TYPE val) __attribute__((always_inline)) {
		div<TYPE>(this->raw, val, 3);
		return *this;
	}

	inline TRGBA& operator%= (const uint8_t ratio) __attribute__((always_inline)) {
		::scale8(this->raw, ratio, 3);
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

	/* ------------------- Other ----------------- */

#ifdef ARDUINO
	void print() {
		Serial.print("(");
		Serial.print(0);
		for (int i = 1; i < sizeof(*this); i++) {
			Serial.print(", ");
			Serial.print(this->raw[i]);
		}
		Serial.print(")");
	}

	void println() {
		print();
		Serial.println();
	}
#endif
};

/* --------------- Blending ---------------*/

template <typename TYPE>
TRGBA<TYPE> blendCOPY(TRGBA<TYPE> &lhs, const TRGBA<TYPE> &rhs);

template <typename TYPE>
TRGB<TYPE> blendCOPY(TRGB<TYPE> &lhs, const TRGBA<TYPE> &rhs);

template <>
inline TRGBA<uint8_t> blendCOPY(TRGBA<uint8_t> &lhs, const TRGBA<uint8_t> &rhs) {
	uint8_t a = lhs.a;
	lhs.lerp8(rhs, rhs.a);
	lhs.a = a;
	return lhs;
}

template <>
inline TRGB<uint8_t> blendCOPY(TRGB<uint8_t> &lhs, const TRGBA<uint8_t> &rhs) {
	lhs.lerp8(rhs, rhs.a);
	return lhs;
}

template <typename TYPE>
TRGBA<TYPE> blendADD(TRGBA<TYPE> &lhs, const TRGBA<TYPE> &rhs);

template <typename TYPE>
TRGB<TYPE> blendADD(TRGB<TYPE> &lhs, const TRGBA<TYPE> &rhs);

template <>
inline TRGBA<uint8_t> blendADD(TRGBA<uint8_t> &lhs, const TRGBA<uint8_t> &rhs) {
	TRGBA<uint8_t> adj = TRGBA<uint8_t>(rhs).scale8(rhs.a);
	lhs += adj;
	return lhs;
}

template <>
inline TRGB<uint8_t> blendADD(TRGB<uint8_t> &lhs, const TRGBA<uint8_t> &rhs) {
	lhs += rhs;
	return lhs;
}

/*
inline CRGB applyCOPYTo(CRGB &other) __attribute__((always_inline)) {
	return other.lerp8((CRGB &)*this, this->a);
}

inline RGBA applyADDTo(RGBA &other) __attribute__((always_inline)) {
	RGBA ret = this->nscale8(this->a);
	ret.a = this->a;
	ret += other;
	
	return ret;
}

inline CRGB applyADDTo(CRGB &other) __attribute__((always_inline)) {
	RGBA adj = this->nscale8(this->a);
	return other + (CRGB &)adj;
}
*/

};

