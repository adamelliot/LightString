#pragma once

namespace LightString {

template <template <typename> class T, typename FORMAT = uint8_t>
class TMappingPixelBuffer2d : public TPixelBufferAdapter<T, FORMAT> {/*
private:
	// Raw pixels points at pixels - 1. The space right before the buffer
	// is used as a dummy pixel that can be mapped to for things we don't
	// actually want to see.
	T<FORMAT> *rawPixels;
*/
protected:
	TPixelBuffer<T, FORMAT> &buffer;

public:

	uint16_t width = 1, height = 1;

	TMappingPixelBuffer2d(TPixelBuffer<T, FORMAT> &buffer) : buffer(buffer) {}
	TMappingPixelBuffer2d(TPixelBuffer<T, FORMAT> &buffer, const uint16_t width, const uint16_t height)
		: buffer(buffer), width(width), height(height) {}

   	virtual ~TMappingPixelBuffer2d() {}
/*
	// Length is not necessarily proportionate to width & height
	// as the mapping will put pixels where ever it choses in the buffer
	// So we need to have a separate length
	TMappingPixelBuffer2d(const uint16_t width, const uint16_t height, const uint16_t length)
		: TPixelBuffer<T, FORMAT>(length, true), width(width), height(height) {}

	TMappingPixelBuffer2d(const uint16_t width, const uint16_t height)
		: TMappingPixelBuffer2d<T, FORMAT>(width, height, width * height) {}

	// Pixels here should represent the whole plane + 1 pixel
	TMappingPixelBuffer2d(T<FORMAT> *pixels, const uint16_t width, const uint16_t height)
		: TPixelBuffer<T, FORMAT>(pixels, width * height + 1, true), width(width), height(height) {}

	using TPixelBuffer<T, FORMAT>::resize;

	virtual bool resize(uint16_t width, uint16_t height) {
		this->width = width;
		this->height = height;

		return this->resize(width * height);
	}
*/

	TPixelBuffer<T, FORMAT> &getBuffer() { return buffer; }

	/* -------------- 2d Methods ---------------- */

	virtual int16_t xy(int16_t x, int16_t y) {
		if (x < 0 || y < 0) return -1;
		return (y * width) + x;
	}

	inline void setPixel(uint16_t x, uint16_t y, T<FORMAT> col) __attribute__((always_inline)) {
		this->pixels[xy(x, y)] = col;
	}

	inline void vertLine(int16_t x, int16_t y, int16_t len, T<FORMAT> col) {
		if (len < 0) {
			y += (len + 1);
			len *= -1;
		}

		for (int i = 0; i < len; i++)
			this->pixels[xy(x, y + i)] = col;
	}

	inline void horzLine(int16_t x, int16_t y, int16_t len, T<FORMAT> col) {
		if (len < 0) {
			x += (len + 1);
			len *= -1;
		}

		for (int i = 0; i < len; i++)
			this->pixels[xy(x + i, y)] = col;
	}

	inline void drawRect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, T<FORMAT> col) {
		if (y0 > y1) std::swap(y0, y1);

		for (; y0 < y1; y0++) {
			horzLine(x0, y0, x1 - x0, col);
		}
	}

	template <typename TYPE>
	inline void drawRect(TPoint<TYPE, 2> pt, uint16_t size, T<FORMAT> col) {
		drawRect(pt.x, pt.y, pt.x + size, pt.y + size, col);
	}

	void lineTo(int16_t x0, int16_t y0, int16_t x1, int16_t y1, T<FORMAT> col) {
		//	if (x0 >= kMatrixWidth || y0 >= kMatrixHeight || x1 >= kMatrixWidth || y1 >= kMatrixHeight)
		//	return;

		if (x0 == x1) {
			y1 += y1 < y0 ? -1 : 1;
			vertLine(x0, y0, y1 - y0, col);
		} else if (y0 == y1) {
			x1 += x1 < x0 ? -1 : 1;
			horzLine(x0, y0, x1 - x0, col);
		} else {
			int e;
			signed int dx, dy, j, temp;
			signed char s1, s2, xchange;
			signed int x, y;

			x = x0;
			y = y0;

			//take absolute value
			if (x1 < x0) {
				dx = x0 - x1;
				s1 = -1;
			}
			else if (x1 == x0) {
				dx = 0;
				s1 = 0;
			}
			else {
				dx = x1 - x0;
				s1 = 1;
			}

			if (y1 < y0) {
				dy = y0 - y1;
				s2 = -1;
			}
			else if (y1 == y0) {
				dy = 0;
				s2 = 0;
			}
			else {
				dy = y1 - y0;
				s2 = 1;
			}

			xchange = 0;

			if (dy > dx) {
				temp = dx;
				dx = dy;
				dy = temp;
				xchange = 1;
			}

			e = ((int)dy << 1) - dx;

			for (j = 0; j <= dx; j++) {
				this->pixels[xy(x, y)] = col;

				if (e >= 0) {
					if (xchange == 1) x = x + s1;
					else y = y + s2;
					e = e - ((int)dx << 1);
				}
				if (xchange == 1)
					y = y + s2;
				else
					x = x + s1;
				e = e + ((int)dy << 1);
			}
		}
	}

	template <typename TYPE>
	inline void lineTo(TPoint<TYPE, 2> pt1, TPoint<TYPE, 2> pt2, T<FORMAT> col) {
		lineTo(pt1.x, pt1.y, pt2.x, pt2.y, col);
	}
};

};