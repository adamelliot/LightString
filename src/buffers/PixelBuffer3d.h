#pragma once

#include <math.h>
#include <cmath>

namespace LightString {

template <template <typename> class T, typename FORMAT = uint8_t>
class TPixelBuffer3d : public TPixelBuffer<T, FORMAT> {
	typedef T<FORMAT> (* BlendOperator)(T<FORMAT> &, const T<FORMAT> &);

public:
	uint32_t width = 1, height = 1, depth = 1;

	TPixelBuffer3d(const uint32_t length)
		: TPixelBuffer<T, FORMAT>(length, true) {}

	TPixelBuffer3d(const uint32_t width, const uint32_t height, const uint32_t depth, const uint32_t length)
		: TPixelBuffer<T, FORMAT>(length, true), width(width), height(height), depth(depth) {}

	TPixelBuffer3d(const uint32_t width, const uint32_t height, const uint32_t depth)
		: TPixelBuffer3d(width, height, depth, width * height * depth) {}

	// Pixels here should represent the whole space + 1 pixel
	TPixelBuffer3d(T<FORMAT> *pixels, const uint32_t width, const uint32_t height, const uint32_t depth)
		: TPixelBuffer<T, FORMAT>(pixels, width * height * depth, true), width(width), height(height), depth(depth)
	{}

	TPixelBuffer3d(T<FORMAT> *pixels, const uint32_t size)
		: TPixelBuffer<T, FORMAT>(pixels, size, true), width(1), height(1), depth(1)
	{}

	virtual ~TPixelBuffer3d() {}

	using TPixelBuffer<T, FORMAT>::resize;

	bool resize(uint32_t width, uint32_t height, uint8_t depth) {
		this->width = width;
		this->height = height;
		this->depth = depth;

		return this->resize(width * height * depth);
	}

	virtual int xyz(int x, int y, int z) {
		if (x < 0 || y < 0 || z < 0) return -1;

		return (x + (y * width) + (z * width * height));
	}

	/* --------------- 2d Methods ----------------- */

	inline void setPixel(uint32_t x, uint32_t y, T<FORMAT> col) __attribute__((always_inline)) {
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

/**
 * The Mapping Pixel Buffer 3d assumes rectilinear separation
 * of points in 3d space which then treats the spaces as a voxel
 * volume with associated drawing routines.
 * It uses a mapping
 */
template <template <typename> class T, typename FORMAT = uint8_t>
class TMappingPixelBuffer3d : public TPixelBuffer3d<T, FORMAT> {

private:
	std::vector<int16_t> mapping;

public:
	TMappingPixelBuffer3d(const uint32_t length) : TPixelBuffer3d<T, FORMAT>(length) {}

	TMappingPixelBuffer3d(const uint32_t width, const uint32_t height, const uint32_t depth, const uint32_t length)
		: TPixelBuffer3d<T, FORMAT>(width, height, depth, length) {}

	TMappingPixelBuffer3d(const uint32_t width, const uint32_t height, const uint32_t depth)
		: TPixelBuffer3d<T, FORMAT>(width, height, depth, width * height * depth) {}

	TMappingPixelBuffer3d(T<FORMAT> *pixels, const uint32_t width, const uint32_t height, const uint32_t depth)
		: TPixelBuffer3d<T, FORMAT>(pixels, width, height, depth) {}

	TMappingPixelBuffer3d(T<FORMAT> *pixels, const uint32_t size)
		: TPixelBuffer3d<T, FORMAT>(pixels, size) {}

	template<typename TYPE = uint32_t>
	TMappingPixelBuffer3d(const TPointMapping<TYPE> &mapping) : TPixelBuffer3d<T, FORMAT>(1) {
		uint32_t w = std::ceil(mapping.bounds.width);
		uint32_t h = std::ceil(mapping.bounds.height);
		uint32_t d = std::ceil(mapping.bounds.depth);
		this->resize(w, h, d);
		setMapping(mapping);
	}

	virtual ~TMappingPixelBuffer3d() {}

	/**
	 * The actual size of the buffer should be adjusted to fit the mapping.
	 * Any points in the mapping that fall outside the bounds of the
	 * actual LED buffer will be mapping to blank space (not-drawn).
	 */
	template<typename TYPE = uint32_t>
	void setMapping(const TPointMapping<TYPE> &newMapping) {
		this->width  = std::ceil(newMapping.bounds.width + 1);
		this->height = std::ceil(newMapping.bounds.height + 1);
		this->depth  = std::ceil(newMapping.bounds.depth + 1);

		auto &w = this->width;
		auto &h = this->height;
		auto &d = this->depth;

		auto size = w * h * d;
		mapping.clear();
		mapping.resize(size);

		std::fill(mapping.begin(), mapping.end(), -1);

		for (auto &point : newMapping.points) {
			if (point.index >= this->getSize()) continue;
			mapping[point.x + (point.y * w) + (point.z * w * h)] = point.index;
		}
	}

	virtual int xyz(int x, int y, int z) {
		if (x < 0 || y < 0 || z < 0) return -1;
		auto &w = this->width;
		auto &h = this->height;
		auto &d = this->depth;

		if (x >= w || y >= h || z >= d) return -1;
		return mapping[x + (y * w) + (z * w * h)];
	}
};

};