#pragma once

#include <math.h>
#include <cmath>

namespace LightString {

template <template <typename> class T, typename FORMAT = uint8_t>
class TMappingPixelBuffer3d : public TPixelBuffer<T, FORMAT> {
	typedef T<FORMAT> (* BlendOperator)(T<FORMAT> &, const T<FORMAT> &);

private:
	T<FORMAT> *rawPixels;

public:
	uint16_t width = 1, height = 1, depth = 1;

	inline TMappingPixelBuffer3d(const uint16_t length)
		: TPixelBuffer<T, FORMAT>(length + 1) {
		rawPixels = this->pixels;
		this->pixels++;
		this->length--;
	}

	inline TMappingPixelBuffer3d(const uint16_t width, const uint16_t height, const uint16_t depth, const uint16_t length)
		: TPixelBuffer<T, FORMAT>(length + 1), width(width), height(height), depth(depth) {
		rawPixels = this->pixels;
		this->pixels++;
		this->length--;
	}

	inline TMappingPixelBuffer3d(const uint16_t width, const uint16_t height, const uint16_t depth)
		: TMappingPixelBuffer3d(width, height, depth, width * height * depth) {}

	// Pixels here should represent the whole space + 1 pixel
	inline TMappingPixelBuffer3d(T<FORMAT> *pixels, const uint16_t width, const uint16_t height, const uint16_t depth)
		: TPixelBuffer<T, FORMAT>(pixels + 1, width * height * depth), width(width), height(height), depth(depth)
	{
		this->rawPixels = pixels;
	}

	virtual inline ~TMappingPixelBuffer3d() {
		// Put pixels back so it deletes properly.
		this->pixels = rawPixels;
	}

	inline bool resize(uint16_t length) {
		if (!this->shouldDelete && length > 0) {
#ifdef ARDUINO
			Serial.println("ERROR: Cannot resize buffer that is not owned by pixel buffer.");
#else
			fprintf(stderr, "ERROR: Cannot resize buffer that is not owned by pixel buffer.\n");
#endif
			return false;
		}

		if (this->shouldDelete) {
			delete this->rawPixels;
		}

		this->width = 1;
		this->height = 1;
		this->depth = 1;

		this->length = length;
		this->pixels = new T<FORMAT>[length + 1];
		memset(this->pixels, 0, sizeof(T<FORMAT>) * length);
		rawPixels = this->pixels;
		this->pixels++;

		return true;
	}

	virtual int16_t xyz(int16_t x, int16_t y, int16_t z) {
		if (x < 0 || y < 0 || z < 0) return -1;

		return (x + (y * width) + (z * width * height));
	}

	/* --------------- 2d Methods ----------------- */

	inline void setPixel(uint16_t x, uint16_t y, T<FORMAT> col) __attribute__((always_inline)) {
		this->pixels[xyz(x, y, 0)] = col;
	}

	/* --------------- 3d Methods ----------------- */

	/* ---------- Anti-aliased Methods ------------ */
	// These all assume that we have an alpha channel

	template <BlendOperator BLEND_OP = blendCOPY>
	void drawPixel(const Point3f &pt, T<FORMAT> col) {
		float dx0, dy0, dz0;
		float dx1, dy1, dz1;
		dx1 = modf(pt.x, &dx0);
		dy1 = modf(pt.y, &dy0);
		dz1 = modf(pt.z, &dz0);

		int16_t x = dx0;
		int16_t y = dy0;
		int16_t z = dz0;

		dx0 = 1.0 - dx1;
		dy0 = 1.0 - dy1;
		dz0 = 1.0 - dz1;
		float a = col.a;

		col.a = dx0 * dy0 * dz0 * a;
		BLEND_OP(this->pixels[xyz(x + 0, y + 0, z + 0)], col);

		col.a = dx1 * dy0 * dz0 * a;
		BLEND_OP(this->pixels[xyz(x + 1, y + 0, z + 0)], col);

		col.a = dx1 * dy1 * dz0 * a;
		BLEND_OP(this->pixels[xyz(x + 1, y + 1, z + 0)], col);

		col.a = dx1 * dy1 * dz1 * a;
		BLEND_OP(this->pixels[xyz(x + 1, y + 1, z + 1)], col);

		col.a = dx0 * dy1 * dz1 * a;
		BLEND_OP(this->pixels[xyz(x + 0, y + 1, z + 1)], col);

		col.a = dx0 * dy0 * dz1 * a;
		BLEND_OP(this->pixels[xyz(x + 0, y + 0, z + 1)], col);

		col.a = dx1 * dy0 * dz1 * a;
		BLEND_OP(this->pixels[xyz(x + 1, y + 0, z + 1)], col);

		col.a = dx0 * dy1 * dz0 * a;
		BLEND_OP(this->pixels[xyz(x + 0, y + 1, z + 0)], col);
	}

	/**
	 * Draw a plane that fits into one "slice" of pixel data at an X value
	 *
	 * TODO: Handle case where size is < 1.0
	 */
	template <BlendOperator BLEND_OP = blendCOPY>
	inline void drawRectX(Point2f pt0, Point2f pt1, int16_t x, int16_t x1, T<FORMAT> col) {
		float &y0 = pt0.x, &y1 = pt1.x;
		float &z0 = pt0.y, &z1 = pt1.y;

		if (y0 > y1) std::swap(y0, y1);
		if (z0 > z1) std::swap(z0, z1);
		if (x  > x1) std::swap(x, x1);

		float iy0, iy1, iz0, iz1;

		// Fractional start
		float dy0 = 1.0 - modf(y0, &iy0);
		float dz0 = 1.0 - modf(z0, &iz0);

		// Fractional end
		float dy1 = modf(y1, &iy1);
		float dz1 = modf(z1, &iz1);

		float a = col.a;

		for (; x < x1; x++) {
			// Draw corners
			col.a = a * dy0 * dz0;
			BLEND_OP(this->pixels[xyz(x, iy0, iz0)], col);
			col.a = a * dy1 * dz0;
			BLEND_OP(this->pixels[xyz(x, iy1, iz0)], col);
			col.a = a * dy1 * dz1;
			BLEND_OP(this->pixels[xyz(x, iy1, iz1)], col);
			col.a = a * dy0 * dz1;
			BLEND_OP(this->pixels[xyz(x, iy0, iz1)], col);

			float y0_1 =  y0 + 1;
			float z0_1 =  z0 + 1;

			// Draw frame
			col.a = a * dz0;
			lineY_nc<BLEND_OP>(x, y0_1, z0, y1, col);
			col.a = a * dy0;
			lineZ_nc<BLEND_OP>(x, y0, z0_1, z1, col);
			col.a = a * dy1;
			lineZ_nc<BLEND_OP>(x, y1, z0_1, z1, col);
			col.a = a * dz1;
			lineY_nc<BLEND_OP>(x, y0_1, z1, y1, col);

			// Draw inside
			col.a = a;
			rectX_nc<BLEND_OP>(x, y0_1, z0_1, y1, z1, col);
		}
	}

	template <BlendOperator BLEND_OP = blendCOPY>
	inline void drawRectX(Point2f pt0, Point2f pt1, int16_t x, T<FORMAT> col) {
		drawRectX<BLEND_OP>(pt0, pt1, x, x + 1, col);
	}

	template <BlendOperator BLEND_OP = blendCOPY>
	inline void drawCuboid(Point3f pt0, Point3f pt1, T<FORMAT> col) {
		float &x0 = pt0.x, &x1 = pt1.x;
		float &y0 = pt0.y, &y1 = pt1.y;
		float &z0 = pt0.z, &z1 = pt1.z;

		if (x0 > x1) std::swap(x0, x1);
		if (y0 > y1) std::swap(y0, y1);
		if (z0 > z1) std::swap(z0, z1);

		float ix0, ix1;
		float dx0 = 1.0 - modf(x0, &ix0);
		float dx1 = modf(x1, &ix1);

		float a = col.a;

		auto pt0_ = Point2f(pt0.y, pt0.z);
		auto pt1_ = Point2f(pt1.y, pt1.z);

		col.a = a * dx0;
		drawRectX<BLEND_OP>(pt0_, pt1_, ix0, col);

		col.a = a;
		drawRectX<BLEND_OP>(pt0_, pt1_, ix0 + 1, ix1, col);

		col.a = a * dx1;
		drawRectX<BLEND_OP>(pt0_, pt1_, ix1, col);
	}

	template <BlendOperator BLEND_OP = blendCOPY>
	inline void drawCuboid(TCuboid<float> cuboid, T<FORMAT> col) {
		drawCuboid<BLEND_OP>(Point3f(cuboid.x, cuboid.y, cuboid.z),
			Point3f(cuboid.x + cuboid.width, cuboid.y + cuboid.height, cuboid.z + cuboid.depth));
	}

	template <BlendOperator BLEND_OP = blendCOPY>
	inline void drawCube(Point3f pt0, float size, T<FORMAT> col) {
		Point3f pt1 = pt0;
		pt1 += size;
		drawCuboid<BLEND_OP>(pt0, pt1, col);
	}

	/* --------- Basic aliased Methods --------- */

	template <BlendOperator BLEND_OP = blendCOPY>
	inline void drawPixel(Point3i pt, T<FORMAT> col) {
		BLEND_OP(this->pixels[xyz(pt.x, pt.y, pt.z)], col);
	}

	template <BlendOperator BLEND_OP = blendCOPY>
	inline void lineX_nc(int16_t x, int16_t y, int16_t z, int16_t x1, T<FORMAT> col) {
		for (; x < x1; x++) BLEND_OP(this->pixels[xyz(x, y, z)], col);
	}

	template <BlendOperator BLEND_OP = blendCOPY>
	inline void lineY_nc(int16_t x, int16_t y, int16_t z, int16_t y1, T<FORMAT> col) {
		for (; y < y1; y++) BLEND_OP(this->pixels[xyz(x, y, z)], col);
	}

	template <BlendOperator BLEND_OP = blendCOPY>
	inline void lineZ_nc(int16_t x, int16_t y, int16_t z, int16_t z1, T<FORMAT> col) {
		for (; z < z1; z++) BLEND_OP(this->pixels[xyz(x, y, z)], col);
	}

	template <BlendOperator BLEND_OP = blendCOPY>
	inline void rectX_nc(int16_t x, int16_t y, int16_t z, int16_t y1, int16_t z1, T<FORMAT> col) {
		for (; y < y1; y++) lineZ_nc<BLEND_OP>(x, y, z, z1, col);
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

	template <BlendOperator BLEND_OP = blendCOPY>
	inline void drawCuboid(Point3i pt1, Point3i pt2, T<FORMAT> col) {
		if (pt1.x > pt2.x) std::swap(pt1.x, pt2.x);
		if (pt1.y > pt2.y) std::swap(pt1.y, pt2.y);
		if (pt1.z > pt2.z) std::swap(pt1.z, pt2.z);

		for (int16_t x = pt1.x; x < pt2.x; x++) {
			for (int16_t y = pt1.y; y < pt2.y; y++) {
				for (int16_t z = pt1.z; z < pt2.z; z++) {
					BLEND_OP(this->pixels[xyz(x, y, z)], col);
				}
			}
		}
	}

	template <BlendOperator BLEND_OP = blendCOPY>
	inline void drawCube(Point3i pt, int size, T<FORMAT> col) {
		Point3i pt1 = pt;
		pt1 += size;
		drawCuboid<BLEND_OP>(pt, pt1, col);
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