#pragma once

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

	inline TRGB(const CRGB &rhs) __attribute__((always_inline)) {
		this->r = rhs.r;
		this->g = rhs.g;
		this->b = rhs.b;
	}
	
	/* --------------- Casting ----------------- */

	inline operator bool() const __attribute__((always_inline)) {
		return r || g || b;
	}

	/* -------------- Operators ---------------- */

	inline TRGB& operator= (const TRGB<TYPE> &rhs) __attribute__((always_inline)) {
		this->r = rhs.r;
		this->g = rhs.g;
		this->b = rhs.b;
		
		return *this;
	}
	
	inline TRGB& operator+= (const TRGB<TYPE> &rhs) __attribute__((always_inline)) {
		qadd8(this->raw, rhs.raw, sizeof(*this));
		return *this;
	}

	inline TRGB& operator+= (const TYPE val) __attribute__((always_inline)) {
		qadd8(this->raw, val, sizeof(*this));
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
		qsub8(this->raw, rhs.raw, sizeof(*this));
		return *this;
	}

	inline TRGB& operator-= (const TYPE val) __attribute__((always_inline)) {
		qsub8(this->raw, val, sizeof(*this));
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
		qmul8(this->raw, val, sizeof(*this));
		return *this;
	}

	inline TRGB& operator/= (const TYPE val) __attribute__((always_inline)) {
		div<TYPE>(this->raw, val, sizeof(*this));
		return *this;
	}

	inline TRGB& operator%= (const uint8_t ratio) __attribute__((always_inline)) {
		::scale8(this->raw, ratio, sizeof(*this));
		return *this;
	}

	/* ----------- Color Manipulation ------------ */

	inline TRGB &lerp(const TRGB<TYPE> &other, const float ratio) __attribute__((always_inline)) {
		if (ratio >= 1) {
			*this = other;
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

	inline TRGB &scale8(const uint8_t ratio) {
		::scale8(this->raw, ratio, sizeof(*this));
		return *this;
	}

	inline TRGB &maximizeBrightness(const TYPE max) {
		return *this;
	}

	inline TRGB &maximizeBrightness() {
		return maximizeBrightness(255);
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

/* --------------- Specializations -----------------*/ 

template <>
inline TRGB<uint8_t>& TRGB<uint8_t>::maximizeBrightness(const uint8_t limit) {
	uint8_t max = r;
	if (g > max) max = g;
	if (b > max) max = b;
	uint16_t fact = ((uint16_t)limit * 256) / max;

	r = (r * fact) / 256;
	g = (g * fact) / 256;
	b = (b * fact) / 256;

	return *this;
}

template <>
inline TRGB<float>& TRGB<float>::maximizeBrightness(const float limit) {
	float max = r;
	if (g > max) max = g;
	if (b > max) max = b;
	uint16_t fact = 1 / max;

	r *= fact;
	g *= fact;
	b *= fact;

	return *this;
}

template <>
inline TRGB<uint8_t>& TRGB<uint8_t>::maximizeBrightness() {
	return maximizeBrightness(255);
}

template <>
inline TRGB<float>& TRGB<float>::maximizeBrightness() {
	return maximizeBrightness(1);
}

