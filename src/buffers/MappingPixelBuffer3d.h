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

	// Pixels here should represent the whole plane + 1 pixel
	inline TMappingPixelBuffer3d(T<FORMAT> *pixels, const uint16_t width, const uint16_t height, const uint16_t depth)
		: TPixelBuffer<T, FORMAT>(pixels + 1, width * height * depth), width(width), height(height), depth(depth)
	{
		this->rawPixels = pixels;
	}

	virtual inline ~TMappingPixelBuffer3d() {
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

	inline void rect(int16_t x1, int16_t y1, int16_t z1, int16_t x2, int16_t y2, int16_t z2, T<FORMAT> col) {
		if (x1 > x2) std::swap(x1, x2);
		if (y1 > y2) std::swap(y1, y2);
		if (z1 > z2) std::swap(z1, z2);

		for (int16_t x = x1; x < x2; x++) {
			for (int16_t y = y1; y < y2; y++) {
				for (int16_t z = z1; z < z2; z++) {
					this->pixels[xyz(x, y, z)] = col;
				}
			}
		}
	}

	inline void rect(int16_t x1, int16_t y1, int16_t z1, int16_t size, T<FORMAT> col) {
		rect(x1, y1, z1, x1 + size, y1 + size, z1 + size, col);
	}

	// 3d Bresenham Line modified from: http://www.ict.griffith.edu.au/anthony/info/graphics/bresenham.procs
	void lineTo(int16_t x1, int16_t y1, int16_t z1, const int16_t x2, const int16_t y2, const int16_t z2, T<FORMAT> col) {
		int16_t i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;

		dx = x2 - x1;
		dy = y2 - y1;
		dz = z2 - z1;
		x_inc = (dx < 0) ? -1 : 1;
		l = abs(dx);
		y_inc = (dy < 0) ? -1 : 1;
		m = abs(dy);
		z_inc = (dz < 0) ? -1 : 1;
		n = abs(dz);
		dx2 = l << 1;
		dy2 = m << 1;
		dz2 = n << 1;

		if ((l >= m) && (l >= n)) {
			err_1 = dy2 - l;
			err_2 = dz2 - l;
			for (i = 0; i < l; i++) {
				this->pixels[xyz(x1, y1, z1)] = col;
				if (err_1 > 0) {
					y1 += y_inc;
					err_1 -= dx2;
				}
				if (err_2 > 0) {
					z1 += z_inc;
					err_2 -= dx2;
				}
				err_1 += dy2;
				err_2 += dz2;
				x1 += x_inc;
			}
		} else if ((m >= l) && (m >= n)) {
			err_1 = dx2 - m;
			err_2 = dz2 - m;
			for (i = 0; i < m; i++) {
				this->pixels[xyz(x1, y1, z1)] = col;
				if (err_1 > 0) {
					x1 += x_inc;
					err_1 -= dy2;
				}
				if (err_2 > 0) {
					z1 += z_inc;
					err_2 -= dy2;
				}
				err_1 += dx2;
				err_2 += dz2;
				y1 += y_inc;
			}
		} else {
			err_1 = dy2 - n;
			err_2 = dx2 - n;
			for (i = 0; i < n; i++) {
				this->pixels[xyz(x1, y1, z1)] = col;
				if (err_1 > 0) {
					y1 += y_inc;
					err_1 -= dz2;
				}
				if (err_2 > 0) {
					x1 += x_inc;
					err_2 -= dz2;
				}
				err_1 += dy2;
				err_2 += dx2;
				z1 += z_inc;
			}
		}
		this->pixels[xyz(x1, y1, z1)] = col;
	}
};

};