#pragma once

#ifndef ARDUINO
#include <stdio.h>
#endif

#ifdef USE_FASTLED
extern void hsv2rgb_rainbow(CHSV *hsv, CRGB *rgb);
#endif

namespace LightString {

template <typename TYPE>
struct THSV {
	union {
		struct {
			union {
				TYPE h;
				TYPE hue;
			};
			union {
				TYPE s;
				TYPE sat;
				TYPE saturation;
			};
			union {
				TYPE v;
				TYPE val;
				TYPE value;
			};
		};
		TYPE raw[3];
	};

	inline THSV() : h(0), s(0), v(0) {}
	inline THSV(TYPE h, TYPE s, TYPE v) : h(h), s(s), v(v) {}

	inline THSV(const THSV<TYPE> &rhs) __attribute__((always_inline)) {
		this->h = rhs.h;
		this->s = rhs.s;
		this->v = rhs.v;
	}

	inline THSV(const TRGB<TYPE> &rhs) {
		*this = rhs.toHSV();
	}

	inline THSV& operator= (const THSV<TYPE> &rhs) __attribute__((always_inline)) {
		this->h = rhs.h;
		this->s = rhs.s;
		this->v = rhs.v;

		return *this;
	}

	// Ranges (uint8_t): All values 0 - 255
	inline TRGB<TYPE> toRGB() const {
		TRGB<TYPE> ret;
		hsv2rgb(*this, ret);
		return ret;
	}
};

/* --------------- Specializations --------------- */

template <typename TYPE>
inline TRGB<TYPE>::TRGB(const THSV<TYPE> &rhs) {
	auto rgb = rhs.toRGB();
	this->r = rgb.r;
	this->g = rgb.g;
	this->b = rgb.b;
}

template <>
inline TRGBA<uint8_t>::TRGBA(const THSV<uint8_t> &rhs) {
	auto rgb = rhs.toRGB();
	this->r = rgb.r;
	this->g = rgb.g;
	this->b = rgb.b;
	this->a = 0xff;
}

template <>
inline TRGBA<float>::TRGBA(const THSV<float> &rhs) {
	auto rgb = rhs.toRGB();
	this->r = rgb.r;
	this->g = rgb.g;
	this->b = rgb.b;
	this->a = 1.0f;
}

template <typename TYPE>
inline THSV<TYPE> TRGB<TYPE>::toHSV() const {
	THSV<TYPE> ret;
	rgb2hsv(*this, ret);
	return ret;
}

/* -------------- Conversion Methods ----------------- */

// Ranges: h 0 - 360, s = 0 - 1, v = 0 - 1
inline void hsv2rgb(const THSV<float> &in, TRGB<float> &out) {
	float      hh, p, q, t, ff;
	long       i;

	if (in.s <= 0.0) {      // < is bogus, just shuts up warnings
		out.r = in.v;
		out.g = in.v;
		out.b = in.v;
		return;
	}

	hh = in.h;
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = in.v * (1.0 - in.s);
	q = in.v * (1.0 - (in.s * ff));
	t = in.v * (1.0 - (in.s * (1.0 - ff)));

	switch (i) {
	case 0:
		out.r = in.v;
		out.g = t;
		out.b = p;
		break;
	case 1:
		out.r = q;
		out.g = in.v;
		out.b = p;
		break;
	case 2:
		out.r = p;
		out.g = in.v;
		out.b = t;
		break;

	case 3:
		out.r = p;
		out.g = q;
		out.b = in.v;
		break;
	case 4:
		out.r = t;
		out.g = p;
		out.b = in.v;
		break;
	case 5:
	default:
		out.r = in.v;
		out.g = p;
		out.b = q;
		break;
	}
	return;
}

inline void rgb2hsv(const TRGB<float> &in, THSV<float> &out) {
	float      min, max, delta;

	min = in.r < in.g ? in.r : in.g;
	min = min  < in.b ? min  : in.b;

	max = in.r > in.g ? in.r : in.g;
	max = max  > in.b ? max  : in.b;

	out.v = max;                                // v
	delta = max - min;
	if (delta < 0.00001)
	{
		out.s = 0;
		out.h = 0; // undefined, maybe nan?
		return;
	}
	if ( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
		out.s = (delta / max);                  // s
	} else {
		// if max is 0, then r = g = b = 0
		// s = 0, v is undefined
		out.s = 0.0;
		out.h = 0;                             // its now undefined
		return;
	}
	if ( in.r >= max )                          // > is bogus, just keeps compilor happy
		out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
	else if ( in.g >= max )
		out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
	else
		out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

	out.h *= 60.0;                              // degrees

	if ( out.h < 0.0 )
		out.h += 360.0;

	return;
}

#ifndef USE_FASTLED

// Pulled from FastLED

#define APPLY_DIMMING(X) (X)
#define HSV_SECTION_6 (0x20)
#define HSV_SECTION_3 (0x40)

// hsv2rgb_raw
inline void hsv2rgb(const THSV<uint8_t> &hsv, TRGB<uint8_t> &rgb)
{
	// Convert hue, saturation and brightness ( HSV/HSB ) to RGB
	// "Dimming" is used on saturation and brightness to make
	// the output more visually linear.

	// Apply dimming curves
	uint8_t value = APPLY_DIMMING( hsv.val);
	uint8_t saturation = hsv.sat;

	// The brightness floor is minimum number that all of
	// R, G, and B will be set to.
	uint8_t invsat = APPLY_DIMMING( 255 - saturation);
	uint8_t brightness_floor = (value * invsat) / 256;

	// The color amplitude is the maximum amount of R, G, and B
	// that will be added on top of the brightness_floor to
	// create the specific hue desired.
	uint8_t color_amplitude = value - brightness_floor;

	// Figure out which section of the hue wheel we're in,
	// and how far offset we are withing that section
	uint8_t section = hsv.hue / HSV_SECTION_3; // 0..2
	uint8_t offset = hsv.hue % HSV_SECTION_3;  // 0..63

	uint8_t rampup = offset; // 0..63
	uint8_t rampdown = (HSV_SECTION_3 - 1) - offset; // 63..0

	// We now scale rampup and rampdown to a 0-255 range -- at least
	// in theory, but here's where architecture-specific decsions
	// come in to play:
	// To scale them up to 0-255, we'd want to multiply by 4.
	// But in the very next step, we multiply the ramps by other
	// values and then divide the resulting product by 256.
	// So which is faster?
	//   ((ramp * 4) * othervalue) / 256
	// or
	//   ((ramp    ) * othervalue) /  64
	// It depends on your processor architecture.
	// On 8-bit AVR, the "/ 256" is just a one-cycle register move,
	// but the "/ 64" might be a multicycle shift process. So on AVR
	// it's faster do multiply the ramp values by four, and then
	// divide by 256.
	// On ARM, the "/ 256" and "/ 64" are one cycle each, so it's
	// faster to NOT multiply the ramp values by four, and just to
	// divide the resulting product by 64 (instead of 256).
	// Moral of the story: trust your profiler, not your insticts.

	// Since there's an AVR assembly version elsewhere, we'll
	// assume what we're on an architecture where any number of
	// bit shifts has roughly the same cost, and we'll remove the
	// redundant math at the source level:

	//  // scale up to 255 range
	//  //rampup *= 4; // 0..252
	//  //rampdown *= 4; // 0..252

	// compute color-amplitude-scaled-down versions of rampup and rampdown
	uint8_t rampup_amp_adj   = (rampup   * color_amplitude) / (256 / 4);
	uint8_t rampdown_amp_adj = (rampdown * color_amplitude) / (256 / 4);

	// add brightness_floor offset to everything
	uint8_t rampup_adj_with_floor   = rampup_amp_adj   + brightness_floor;
	uint8_t rampdown_adj_with_floor = rampdown_amp_adj + brightness_floor;


	if ( section ) {
		if ( section == 1) {
			// section 1: 0x40..0x7F
			rgb.r = brightness_floor;
			rgb.g = rampdown_adj_with_floor;
			rgb.b = rampup_adj_with_floor;
		} else {
			// section 2; 0x80..0xBF
			rgb.r = rampup_adj_with_floor;
			rgb.g = brightness_floor;
			rgb.b = rampdown_adj_with_floor;
		}
	} else {
		// section 0: 0x00..0x3F
		rgb.r = rampdown_adj_with_floor;
		rgb.g = rampup_adj_with_floor;
		rgb.b = brightness_floor;
	}
}

inline void rgb2hsv(const TRGB<uint8_t> &in, THSV<uint8_t> &out) {
	fprintf(stderr, "ERROR: rgb2hsv on 8bit not implemented.\n");
}

#else

#warning Need to add mapping to FastLED hsv2rgb

#endif

};