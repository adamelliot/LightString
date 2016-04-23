#ifndef __COLORTYPES_H__
#define __COLORTYPES_H__

/* ==================== Common Helpers =================== */

inline float lerp(const float a, const float b, const float ratio) {
	return (a * (1 - ratio)) + (b * ratio);
}

inline void lerp(float *in, const float *out, uint8_t count, float ratio) {
	for (uint8_t i = 0; i < count; i++) {
		in[i] = lerp(in[i], out[i], ratio);
	}
}

extern uint8_t lerp8by8(uint8_t a, uint8_t b, uint8_t ratio);

inline void lerp8by8(uint8_t *in, const uint8_t *out, uint8_t count, uint8_t ratio) {
	for (uint8_t i = 0; i < count; i++) {
		in[i] = lerp8by8(in[i], out[i], ratio);
	}
}

inline void qadd8(uint8_t *a, const uint8_t *b, uint8_t count) {
	for (uint8_t i = 0; i < count; i++) {
		a[i] = qadd8(a[i], b[i]);
	}
}

inline void qadd8(uint8_t *a, const uint8_t val, uint8_t count) {
	for (uint8_t i = 0; i < count; i++) {
		a[i] = qadd8(a[i], val);
	}
}

inline void qsub8(uint8_t *a, const uint8_t *b, uint8_t count) {
	for (uint8_t i = 0; i < count; i++) {
		a[i] = qsub8(a[i], b[i]);
	}
}

inline void qsub8(uint8_t *a, const uint8_t val, uint8_t count) {
	for (uint8_t i = 0; i < count; i++) {
		a[i] = qsub8(a[i], val);
	}
}

inline void qmul8(uint8_t *a, const uint8_t val, uint8_t count) {
	for (uint8_t i = 0; i < count; i++) {
		a[i] = qmul8(a[i], val);
	}
}

inline void scale8(uint8_t *a, const uint8_t val, uint8_t count) {
	for (uint8_t i = 0; i < count; i++) {
		a[i] = scale8(a[i], val);
	}	
}

template <typename TYPE>
void div(TYPE *in, const TYPE val, uint8_t count) {
	for (uint8_t i = 0; i < count; i++) {
		in[i] /= val;
	}
}

/* ========================= RGB ========================= */

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
};

/* --------------- Specializations -----------------*/ 

template <>
TRGB<uint8_t>& TRGB<uint8_t>::maximizeBrightness(const uint8_t limit) {
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
TRGB<float>& TRGB<float>::maximizeBrightness(const float limit) {
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
TRGB<uint8_t>& TRGB<uint8_t>::maximizeBrightness() {
	return maximizeBrightness(255);
}

template <>
TRGB<float>& TRGB<float>::maximizeBrightness() {
	return maximizeBrightness(1);
}

/* ======================== RGBA ========================= */

template <typename TYPE>
struct TRGBA : TRGB<TYPE> {
	union {
		TYPE a;
		TYPE alpha;
	};

	/* ------------- Constructors ------------ */

	inline TRGBA() __attribute__((always_inline)) : TRGBA<TYPE>(0, 0, 0) {
		this->a = 0;
	}

	inline TRGBA(TYPE r, TYPE g, TYPE b, TYPE a = 0xff) __attribute__((always_inline)) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	/* --------------- Casting ----------------- */

	inline operator TRGB<TYPE>() const {
		return TRGB<TYPE>();
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

	/* ----------- General Utilities ------------ */

	inline TRGBA &lerp(const TRGB<TYPE> &other, uint8_t ratio) __attribute__((always_inline)) {
		uint8_t r8 = ratio * 0xff;
		lerp8by8(this->raw, other.raw, 3, r8);
		return *this;
	}

	inline TRGBA &lerp(const TRGBA<TYPE> &other, uint8_t ratio) __attribute__((always_inline)) {
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
};

/* ============== Actual Declarations ================ */

typedef TRGB<uint8_t> RGBu;
typedef TRGBA<uint8_t> RGBAu;

typedef TRGB<float> RGBf;
typedef TRGBA<float> RGBAf;

#endif