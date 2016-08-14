#pragma once

namespace LightString {

template <template <typename> class T, typename FORMAT = uint8_t>
class TMappingPixelBuffer3d : public TPixelBuffer<T, FORMAT> {
private:
	T<FORMAT> *rawPixels;

public:
	uint16_t width, height, depth;

	inline TMappingPixelBuffer3d(const uint16_t width, const uint16_t height, const uint16_t depth, const uint16_t length)
		: TPixelBuffer<T, FORMAT>(length + 1), width(width), height(height), depth(depth) {
		rawPixels = this->pixels;
		this->pixels++;
		this->length--;
	}

	inline TMappingPixelBuffer3d(const uint16_t width, const uint16_t height, const uint16_t depth)
		: TMappingPixelBuffer3d(width, height, depth, width * height * depth) {}

	inline ~TMappingPixelBuffer3d() {
		// Put pixels back so it deletes properly.
		this->pixels = rawPixels;
	}

	virtual int16_t xyz(int16_t x, int16_t y, int16_t z) {
		if (x < 0 || y < 0 || z < 0) return -1;

		return (x + (y * width) + (z * width * height));
	}

	inline void setPixel(int16_t x, int16_t y, int16_t z, T<FORMAT> col) {
		this->pixels[xyz(x, y, z)] = col;
	}

	inline void lineX(int16_t x, int16_t y, int16_t z, int16_t len, T<FORMAT> col) {
		int8_t step = 1;
		if (len < 0) step = -1;
		x -= step;

		for (; len; len -= step) {
			this->pixels[xyz(x + len, y, z)] = col;
		}
	}

	inline void lineY(int16_t x, int16_t y, int16_t z, int16_t len, T<FORMAT> col) {
		int8_t step = 1;
		if (len < 0) step = -1;
		y -= step;

		for (; len; len -= step) {
			this->pixels[xyz(x, y + len, z)] = col;
		}
	}

	inline void lineZ(int16_t x, int16_t y, int16_t z, int16_t len, T<FORMAT> col) {
		int8_t step = 1;
		if (len < 0) step = -1;
		z -= step;

		for (; len; len -= step) {
			this->pixels[xyz(x, y, z + len)] = col;
		}
	}
};

};