#pragma once

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

#include "colortypes/RGB.h"
#include "colortypes/RGBA.h"

/* ============== Actual Declarations ================ */

typedef TRGB<uint8_t> RGBu;
typedef TRGBA<uint8_t> RGBAu;

typedef TRGB<float> RGBf;
typedef TRGBA<float> RGBAf;

#endif