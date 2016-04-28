#pragma once

#include "RGB.h"

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

	inline TRGBA& operator+= (const TRGBA<TYPE> &rhs) __attribute__((always_inline)) {
		qadd8(this->raw, rhs.raw, sizeof(*this));
		return *this;
	}

	inline TRGBA& operator+= (const TRGB<TYPE> &rhs) __attribute__((always_inline)) {
		qadd8(this->raw, rhs.raw, 3);
		return *this;
	}

	inline TRGBA& operator+= (const TYPE val) __attribute__((always_inline)) {
		qadd8(this->raw, val, sizeof(*this));
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

	inline TRGBA& operator*= (const TYPE val) __attribute__((always_inline)) {
		qmul8(this->raw, val, sizeof(*this));
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
