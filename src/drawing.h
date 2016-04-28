#ifndef _DRAWING_H_
#define _DRAWING_H_

#ifdef USE_FASTLED
#include <FastLED.h>
#endif

#include "colortypes.h"

#define SWAP(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)

extern const uint8_t kMatrixWidth;
extern const uint8_t kMatrixHeight;

extern uint8_t *generic_map;

extern int16_t xy(int16_t, int16_t);

#ifdef USE_FASTLED
void blink(CRGB col = CRGB::Yellow, int t
	imes = 4, int timing = 75);

#endif

void vertLine(CRGB *pixels, int16_t x, int16_t y, int16_t len, CRGB col);
void horzLine(CRGB *pixels, int16_t x, int16_t y, int16_t len, CRGB col);
void lineTo(CRGB *pixels, int16_t x0, int16_t y0, int16_t x1, int16_t y1, CRGB col);

void drawRect(CRGB *pixels, int16_t x0, int16_t y0, int16_t x1, int16_t y1, CRGB col);
void drawSolidCircle(CRGB *pixels, int16_t x, int16_t y, uint8_t radius, CRGB col);

extern void hsv2rgb_rainbow(const CHSV& hsv, CRGB& rgb);

namespace LightString {

struct Rect {
	int16_t x, y, width, height;

	Rect() {}

	Rect(const int16_t x, const int16_t y, const int16_t width, const int16_t height)
		: x(x), y(y), width(width), height(height) {}
};

template <typename T>
struct Vector {
	T x, y;

	Vector() : x(0), y(0) {}
	Vector(const T x, const T y) : x(x), y(y) {}

	inline Vector &operator+=(const Vector<T> &rhs) __attribute__((always_inline))
	{
		this->x += rhs.x;
		this->y += rhs.y;

		return *this;
	}

	inline Vector &operator-=(const Vector<T> &rhs) __attribute__((always_inline))
	{
		this->x -= rhs.x;
		this->y -= rhs.y;

		return *this;
	}

	inline Vector &operator*=(const T &rhs) __attribute__((always_inline))
	{
		this->x *= rhs;
		this->y *= rhs;

		return *this;
	}

	inline Vector &operator/=(const T &rhs) __attribute__((always_inline))
	{
		this->x /= rhs;
		this->y /= rhs;

		return *this;
	}
	
	void print() {
		Serial.print("(");
		Serial.print(x);
		Serial.print(", ");
		Serial.print(y);
		Serial.print(")");
	}
};

template <typename T>
struct Point {
	T x, y;

	Point() : x(0), y(0) {}
	Point(const T x, const T y) : x(x), y(y) {}

	inline Point &operator+=(const Point<T> &rhs) __attribute__((always_inline))
	{
		this->x += rhs.x;
		this->y += rhs.y;

		return *this;
	}

	inline Point &operator+=(const Vector<T> &rhs) __attribute__((always_inline))
	{
		this->x += rhs.x;
		this->y += rhs.y;

		return *this;
	}

	inline Point &operator-=(const Point<T> &rhs) __attribute__((always_inline))
	{
		this->x -= rhs.x;
		this->y -= rhs.y;

		return *this;
	}

	inline Point &operator-=(const Vector<T> &rhs) __attribute__((always_inline))
	{
		this->x -= rhs.x;
		this->y -= rhs.y;

		return *this;
	}
	
	void print() {
		Serial.print("(");
		Serial.print(x);
		Serial.print(", ");
		Serial.print(y);
		Serial.print(")");
	}
};

struct MovingPoint {
	Point<float> origin;
	Vector<float> vel;
	Vector<float> acc;
	float friction;

	MovingPoint() : friction(0) {}
	MovingPoint(float x, float y, float vx = 0, float vy = 0, float ax = 0, float ay = 0, float friction = 0) {
		origin = Point<float>(x, y);
		vel = Vector<float>(vx, vy);
		acc = Vector<float>(ax, ay);
		this->friction = friction;
	}

	inline void update() {
		origin += vel;
		vel += acc;

		if (friction != 0) {
			vel *= 1.0 - friction;
			acc *= 1.0 - friction;
		}
	}
};

struct MovingPoint8 : Point<uint16_t> {
	Point raw;
	Point vec;
	Rect *bounds;

	inline MovingPoint8() {}

	inline MovingPoint8(const int16_t x, const int16_t y, const int16_t vecx, const int16_t vecy, Rect *bounds = NULL) 
		: Point(x, y), raw(x << 8, y << 8), vec(Point(vecx, vecy)), bounds(bounds) {}

	void update() {
		raw += vec;
		if (bounds) {
			int16_t left = bounds->x << 8;
			int16_t top = bounds->y << 8;
			int16_t right = (bounds->x + bounds->width) << 8;
			int16_t bottom = (bounds->y + bounds->height) << 8;

		  if (raw.x >= right) {
		    raw.x = right - (raw.x - right);
		    vec.x *= -1;
		  } else if (raw.x < left) {
		    raw.x = left + (left - raw.x);
		    vec.x *= -1;
		  }

		  if (raw.y >= bottom) {
		    raw.y = bottom - (raw.y - bottom);
		    vec.y *= -1;
		  } else if (raw.y < top) {
		    raw.y = top + (top - raw.y);
		    vec.y *= -1;
		  }
		}

		x = raw.x >> 8;
		y = raw.y >> 8;
	}
};

typedef enum {
	BLEND_COPY = 0,
	BLEND_ADD,
	BLEND_SUBTRACT, // This effect is broken
	BLEND_XOR
	
} EBlendMode;

struct RGB : CRGB {
	inline RGB() __attribute__((always_inline)) {}
	inline RGB(uint8_t r, uint8_t g, uint8_t b) __attribute__((always_inline)) 
		: CRGB(r, g, b) {}

	// Can't set full alpha through this construtor so 3 channel notation works
	inline RGB(uint32_t colorcode) __attribute__((always_inline)) 
		: CRGB(colorcode) {}

	inline RGB(const RGB &rhs) __attribute__((always_inline))
		: CRGB(rhs.r, rhs.g, rhs.b) {}

	inline RGB(const CRGB &rhs) __attribute__((always_inline))
		: CRGB(rhs.r, rhs.g, rhs.b) {}

	inline RGB(const CHSV &rhs) __attribute__((always_inline))
		: CRGB(rhs) {}
	
	RGB& shiftHue(uint8_t by) {
		if (by == 0) return *this;
		
		CHSV hsv = rgb2hsv_approximate(*this);
		hsv.hue += by;
		hsv2rgb_rainbow(hsv, *this);
		return *this;
	}
};

/*
 * RGBA is based on RGB, but adds alpha channel functionality so
 * layering PixelBuffers can properly create composites.
 */
struct RGBA : RGB {
	union {
		uint8_t a;
		uint8_t alpha;
	};

	inline RGBA()  __attribute__((always_inline))
		: RGB(0, 0, 0), a(0) {}
	inline RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff) __attribute__((always_inline)) 
		: RGB(r, g, b), a(a) {}

	// Can't set full alpha through this construtor so 3 channel notation works
	inline RGBA(uint32_t colorcode) __attribute__((always_inline)) 
		: RGB(colorcode), a(0xff)
	{
		if ((colorcode >> 24) != 0) a = (colorcode >> 24) & 0xff;
	}

	inline RGBA(uint32_t colorcode, uint8_t a) __attribute__((always_inline)) 
		: RGB(colorcode), a(a) {}

	inline RGBA(const RGBA &rhs) __attribute__((always_inline))
		: RGB(rhs.r, rhs.g, rhs.b), a(rhs.a) {}

	inline RGBA(const CRGB &rhs) __attribute__((always_inline))
		: RGB(rhs.r, rhs.g, rhs.b), a(0xff) {}

	inline RGBA(const CHSV &rhs) __attribute__((always_inline))
		: a(0xff)
	{
		hsv2rgb_rainbow(rhs, (CRGB &)(*this->raw));
	}

	inline RGBA& operator= (const RGBA& rhs) __attribute__((always_inline)) {
		a = rhs.a;
		this->r = rhs.r;
		this->g = rhs.g;
		this->b = rhs.b;
		return *this;
	}

	inline RGBA& operator+= (const RGBA& rhs)
	{
		this->r = qadd8(r, rhs.r);
		this->g = qadd8(g, rhs.g);
		this->b = qadd8(b, rhs.b);
		this->a = qadd8(a, rhs.a);
		return *this;
	}

	inline RGBA& operator&= (const RGBA& rhs)
	{
		this->r = qadd8(r, ((uint16_t)rhs.r * (uint16_t)(rhs.a)) >> 8);
		this->g = qadd8(g, ((uint16_t)rhs.g * (uint16_t)(rhs.a)) >> 8);
		this->b = qadd8(b, ((uint16_t)rhs.b * (uint16_t)(rhs.a)) >> 8);
		return *this;
	}

	inline RGBA& operator-= (const RGBA& rhs)
	{
		this->r = qsub8(r, rhs.r);
		this->g = qsub8(g, rhs.g);
		this->b = qsub8(b, rhs.b);
		this->a = qsub8(a, rhs.a);
		return *this;
	}
	
  inline RGBA lerp8(RGBA &other, fract8 frac)
  {
    RGBA ret;

    ret.r = lerp8by8(r, other.r, frac);
    ret.g = lerp8by8(g, other.g, frac);
    ret.b = lerp8by8(b, other.b, frac);
    ret.a = lerp8by8(a, other.a, frac);

    return ret;
  }

  inline RGBA lerp8(CRGB other, fract8 frac)
  {
    RGBA ret;

    ret.r = lerp8by8(r, other.r, frac);
    ret.g = lerp8by8(g, other.g, frac);
    ret.b = lerp8by8(b, other.b, frac);
    ret.a = a;

    return ret;
  }
	/*
	RGBA& shiftHue(uint8_t by) {
		if (by == 0) return *this;
		
		CHSV hsv = rgb2hsv_approximate(*(CRGB *)this);
		hsv.hue += by;
		hsv2rgb_rainbow(hsv, *(CRGB *)this);
		return *this;
	}*/
	// Overlay Modes

	// Standard copy mode, but takes Alpha into account
	inline RGBA applyCOPYTo(RGBA &other) __attribute__((always_inline)) {
		RGBA adj = other.lerp8(*this, this->a);
		adj.a = other.a;
		return adj;
	}

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

	inline RGBA applySUBTRACTTo(RGBA &other) __attribute__((always_inline)) {
		RGBA ret = this->nscale8(this->a);
		ret.a = other.a;
		ret -= other;
		
		return ret;
	}

	inline CRGB applySUBTRACTTo(CRGB &other) __attribute__((always_inline)) {
		return other - (CRGB &)*this;
	}

	inline RGBA applyXORTo(RGBA &other) __attribute__((always_inline)) {
		RGBA ret = this->nscale8(this->a);
		ret.a = other.a;

		ret.r ^= other.r;
		ret.g ^= other.g;
		ret.b ^= other.b;

		return ret;
	}

	inline CRGB applyXORTo(CRGB &other) __attribute__((always_inline)) {
		RGBA adj = this->nscale8(this->a);
		return CRGB(adj.r ^ other.r, adj.g ^ other.g, adj.b ^ other.b);
	}
};

typedef struct RGBA Pixel;


};

#endif