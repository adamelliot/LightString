#ifndef _DRAWING_H_
#define _DRAWING_H_

#include "FastLED.h"

#define SWAP(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)

extern const uint8_t kMatrixWidth;
extern const uint8_t kMatrixHeight;

extern uint8_t *generic_map;

extern int16_t xy(int16_t, int16_t);

void blink(CRGB col = CRGB::Yellow, int times = 4, int timing = 75);

void setPixel8(int16_t x, int16_t y, CRGB col);

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
};

/*
 * RGBA is based on RGB, but adds alpha channel functionality so
 * layering PixelBuffers can properly create composites.
 *
 * NOTE: Alpha is preserved on the receive pixel during operations
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
		return *this;
	}

	inline RGBA& operator-= (const RGBA& rhs)
	{
		this->r = qsub8(r, rhs.r);
		this->g = qsub8(g, rhs.g);
		this->b = qsub8(b, rhs.b);
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

  inline RGBA lerp8(CRGB &other, fract8 frac)
  {
    RGBA ret;

    ret.r = lerp8by8(r, other.r, frac);
    ret.g = lerp8by8(g, other.g, frac);
    ret.b = lerp8by8(b, other.b, frac);
    ret.a = a;

    return ret;
  }

	// Overlay Modes

	// Standard copy mode, but takes Alpha into account
	inline RGBA applyCOPYTo(RGBA &other) __attribute__((always_inline)) {
		return RGBA(other.lerp8((CRGB &)*this, this->a), other.a);
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

	inline RGBA applyADDTo(CRGB &other) __attribute__((always_inline)) {
		return other + (CRGB &)*this;
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
		return CRGB(this->r ^ other.r, this->g ^ other.g, this->b ^ other.b);
	}
};

typedef struct RGBA Pixel;

// Place holder type for all generated TPixelBuffers
struct IPixelBuffer {
	virtual uint16_t getLength() = 0;
	virtual void clear() = 0;
};

template <typename T>
struct TPixelBuffer : public IPixelBuffer {
	T *pixels;
	uint16_t length;
	bool shouldDelete;

	inline TPixelBuffer() __attribute__((always_inline))
		: pixels(0), length(0), shouldDelete(false) {}

	inline TPixelBuffer(T *pixels, uint16_t length) __attribute__((always_inline))
		: pixels(pixels), length(length), shouldDelete(false) {}

	inline TPixelBuffer(const uint16_t length) : length(length) {
		pixels = new T[length];
		memset8(pixels, 0, sizeof(T) * length);
		shouldDelete = true;
	}

	inline ~TPixelBuffer() {
		if (shouldDelete) delete pixels;
	}
	
	uint16_t getLength() {
		return length;
	}

	/*
	inline T& operator[] (uint16_t i) __attribute__((always_inline)) {
		return pixels[i];
	}*/

	inline void setPixel(uint16_t index, T col) __attribute__((always_inline)) {
		pixels[index] = col;
	}

	inline void setPixels(uint16_t index, uint8_t length, T col) __attribute__((always_inline)) {
		for (uint16_t i = index; i < index + length; i++) {
			pixels[i] = col;
		}
	}

	inline void setMirroredPixel(uint16_t index, T col) __attribute__((always_inline)) {
		pixels[index] = col;
		pixels[length - index - 1] = col;
	}

	inline void clear() __attribute__((always_inline)) {
		memset8((void*)pixels, 0, sizeof(T) * length);
	}

	inline void showColor(T col) __attribute__((always_inline)) {
		fill_solid(pixels, length, col);
	}

	inline void fade(uint8_t fadeRate) __attribute__((always_inline)) {
		nscale8(pixels, length, fadeRate);
	}
	
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

	inline TPixelBuffer<T> &applyCOPY(TPixelBuffer<RGBA> &src) __attribute__((always_inline)) {
		uint16_t len = min(this->length, src.length);
		for (uint16_t i = 0; i < len; i++) {
			pixels[i] = src.pixels[i].applyCOPYTo(this->pixels[i]);
		}

		return *this;
	}

	inline TPixelBuffer<T> &applyADD(TPixelBuffer<RGBA> &src) __attribute__((always_inline)) {
		uint16_t len = min(this->length, src.length);
		for (uint16_t i = 0; i < len; i++) {
			pixels[i] = src.pixels[i].applyADDTo(this->pixels[i]);
		}

		return *this;
	}

	inline TPixelBuffer<T> &applySUBTRACT(TPixelBuffer<RGBA> &src) __attribute__((always_inline)) {
		uint16_t len = min(this->length, src.length);
		for (uint16_t i = 0; i < len; i++) {
			pixels[i] = src.pixels[i].applySUBTRACTTo(this->pixels[i]);
		}

		return *this;
	}

	inline TPixelBuffer<T> &applyXOR(TPixelBuffer<RGBA> &src) __attribute__((always_inline)) {
		uint16_t len = min(this->length, src.length);
		for (uint16_t i = 0; i < len; i++) {
			pixels[i] = src.pixels[i].applyXORTo(this->pixels[i]);
		}

		return *this;
	}

	inline TPixelBuffer<T> &applyBlend(TPixelBuffer<RGBA> &src, EBlendMode blendMode)  __attribute__((always_inline)) {
		switch (blendMode) {
			case BLEND_COPY: return applyCOPY(src);
			case BLEND_ADD: return applyADD(src);
			case BLEND_SUBTRACT: return applySUBTRACT(src);
			case BLEND_XOR: return applyXOR(src);
		}

		return *this;
	}
};

typedef TPixelBuffer<RGB> RGBBuffer;
typedef TPixelBuffer<RGBA> RGBABuffer;

};

#endif