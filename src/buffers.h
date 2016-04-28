#pragma once

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

	inline T& operator[] (uint16_t index) __attribute__((always_inline)) {
		return pixels[index];
	}

	inline void setPixel(uint16_t index, T col) __attribute__((always_inline)) {
		pixels[index] = col;
	}
	
	inline void setPixelAA(float index, T col) __attribute__((always_inline)) {
		uint16_t base = (uint16_t)index;
		uint8_t ratio = (uint8_t)(fmod(index, 1) * 255);
		if (ratio == 0) {
			setPixel(base, col);
			return;
		}

		pixels[base] = pixels[base].lerp8(col, 255 - ratio);
		if ((base + 1) < length) {
			pixels[base + 1] = pixels[base + 1].lerp8(col, ratio);
		}
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

	inline void fillColor(T col) __attribute__((always_inline)) {
		for( int i = 0; i < length; i++) {
			pixels[i] = col;
		}
	}

	inline void fade(uint8_t fadeRate) __attribute__((always_inline)) {
		for (uint16_t i = 0; i < this->length; i++) {
			pixels[i].nscale8(fadeRate);
		}
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
