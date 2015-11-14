#ifndef _DRAWING_H_
#define _DRAWING_H_

#include "FastLED.h"

#define SWAP(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)

extern const uint8_t kMatrixWidth;
extern const uint8_t kMatrixHeight;

extern uint8_t *generic_map;

extern int16_t xy(int16_t, int16_t);

void setPixel8(int16_t x, int16_t y, CRGB col);

void vertLine(CRGB *pixels, int16_t x, int16_t y, int16_t len, CRGB col);
void horzLine(CRGB *pixels, int16_t x, int16_t y, int16_t len, CRGB col);
void lineTo(CRGB *pixels, int16_t x0, int16_t y0, int16_t x1, int16_t y1, CRGB col);

void drawRect(CRGB *pixels, int16_t x0, int16_t y0, int16_t x1, int16_t y1, CRGB col);
void drawSolidCircle(CRGB *pixels, int16_t x, int16_t y, uint8_t radius, CRGB col);

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


struct Pixel {
	uint8_t a;
	uint8_t r, g, b;

	CRGB toCRGB() {
		return CRGB(r, g, b);
	}
};

struct PixelBuffer {
  CRGB *pixels;
  uint16_t length;
  
	PixelBuffer() : pixels(0), length(0) {}

  PixelBuffer(CRGB *pixels, uint16_t length)
    : pixels(pixels), length(length) {}

  inline void setPixel(uint16_t index, CRGB col) {
    pixels[index] = col;
  }

	inline void setPixels(uint16_t index, uint8_t length, CRGB col) {
		for (uint16_t i = index; i < index + length; i++) {
			pixels[i] = col;
		}
	}

  inline void setMirroredPixel(uint16_t index, CRGB col) {
  	pixels[index] = col;
  	pixels[length - index - 1] = col;
  }

  inline void clear() {
    memset8((void*)pixels, 0, sizeof(struct CRGB) * length);
  }
  
  inline void showColor(CRGB col) {
    fill_solid(pixels, length, col);
  }
  
  inline void fade(uint8_t fadeRate) {
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
};

};

#endif