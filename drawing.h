#ifndef _DRAWING_H_
#define _DRAWING_H_

#include "FastLED.h"

extern const uint8_t kMatrixWidth;
extern const uint8_t kMatrixHeight;

extern uint8_t *generic_map;

int16_t XY(uint8_t, uint8_t);

// Fixed methods are antialiased
// void vertline8(CRGB *pixels, uint16_t x, uint16_t y0, uint16_t y1, CRGB col);
// void horzline8(CRGB *pixels, uint16_t x0, uint16_t x1, uint16_t y, CRGB col);

// Aliased methods
void vertline(CRGB *pixels, uint8_t x, uint8_t y0, uint8_t y1, CRGB col);
void horzline(CRGB *pixels, uint8_t x0, uint8_t x1, uint8_t y, CRGB col);
void lineto(CRGB *pixels, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, CRGB col);


namespace LightString {
  
struct PixelBuffer {
  CRGB *pixels;
  uint16_t length;
  
	PixelBuffer() : pixels(0), length(0) {}
	
  PixelBuffer(CRGB *pixels, uint16_t length)
    : pixels(pixels), length(length) {}

  inline void setPixel(uint16_t index, CRGB col) {
    pixels[index] = col;
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
};

struct Rect {
	int16_t x, y, width, height;
	
	Rect() {}
	
	Rect(const int16_t x, const int16_t y, const int16_t width, const int16_t height)
		: x(x), y(y), width(width), height(height) {}
};

struct Point {
	int16_t x, y;

	Point() : x(0), y(0) {}
	Point(const uint16_t x, const int16_t y) : x(x), y(y) {}

	inline Point &operator+=(const Point &rhs) __attribute__((always_inline))
	{
		this->x += rhs.x;
		this->y += rhs.y;

		return *this;
	}
	
	inline Point &operator-=(const Point &rhs) __attribute__((always_inline))
	{
		this->x -= rhs.x;
		this->y -= rhs.y;

		return *this;
	}
};

struct MovingPoint8 : Point {
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

};

#endif