#pragma once

#ifdef USE_FASTLED
#include <FastLED.h>
#endif

#include "utils.h"
#include "colortypes.h"

#include <vector>
#include <functional>
#include <cmath>

namespace LightString {

template <typename TYPE>
struct TBaseVal2 {
	union {
		struct {
			TYPE x, y;
		};
		TYPE raw[2];
	};
	TBaseVal2() : raw{0, 0} {}
	TBaseVal2(const TYPE x, const TYPE y) : x(x), y(y) {}
};

template <typename TYPE>
struct TBaseVal3 {
	union {
		struct {
			TYPE x, y, z;
		};
		TYPE raw[3];
	};
	TBaseVal3() : raw{0, 0, 0} {}
	TBaseVal3(const TYPE x, const TYPE y, const TYPE z) : x(x), y(y), z(z) {}
};

template <typename TYPE>
struct TBaseVal4 {
	union {
		struct {
			TYPE x, y, z, w;
		};
		TYPE raw[4];
	};
	TBaseVal4() : raw{0, 0, 0, 0} {}
	TBaseVal4(const TYPE x, const TYPE y, const TYPE z, const TYPE w) : x(x), y(y), z(z), w(w) {}
};

template <typename TYPE, unsigned SIZE>
struct TVecBase
	: public std::conditional<SIZE == 4, TBaseVal4<TYPE>,
		typename std::conditional<SIZE == 3, TBaseVal3<TYPE>, TBaseVal2<TYPE>>::type>::type
{
	typedef typename std::conditional<SIZE == 4, TBaseVal4<TYPE>,
		typename std::conditional<SIZE == 3, TBaseVal3<TYPE>, TBaseVal2<TYPE>>::type>::type BASE;
	using BASE::BASE;

	// TODO: This only generates integer values
	void randomize(TYPE min, TYPE max) {
		for (auto i = 0; i < SIZE; i++) {
			this->raw[i] = random(min, max);
		}
	}

	inline TVecBase &operator+=(const TVecBase<TYPE, SIZE> &rhs) {
		for (auto i = 0; i < SIZE; i++) {
			this->raw[i] += rhs.raw[i];
		}
		return *this;
	}

	TVecBase operator+(const TVecBase<TYPE, SIZE> &rhs) const {
		auto ret = *this;
		ret += rhs;
		return ret;
	}

	inline TVecBase &operator-=(const TVecBase<TYPE, SIZE> &rhs) {
		for (auto i = 0; i < SIZE; i++) {
			this->raw[i] -= rhs.raw[i];
		}
		return *this;
	}

	TVecBase operator-(const TVecBase<TYPE, SIZE> &rhs) const {
		auto ret = *this;
		ret -= rhs;
		return ret;
	}

	inline TVecBase &operator+=(const TYPE &rhs) {
		for (auto i = 0; i < SIZE; i++) {
			this->raw[i] += rhs;
		}
		return *this;
	}

	inline TVecBase &operator-=(const TYPE &rhs) {
		for (auto i = 0; i < SIZE; i++) {
			this->raw[i] -= rhs;
		}
		return *this;
	}

	inline TVecBase &operator*=(const TYPE &rhs) {
		for (auto i = 0; i < SIZE; i++) {
			this->raw[i] *= rhs;
		}
		return *this;
	}

	inline TVecBase &operator/=(const TYPE &rhs) {
		for (auto i = 0; i < SIZE; i++) {
			this->raw[i] /= rhs;
		}
		return *this;
	}

	inline TVecBase &operator=(const TYPE &rhs) {
		for (auto i = 0; i < SIZE; i++) {
			this->raw[i] = rhs;
		}
		return *this;
	}

	inline bool operator==(const TVecBase<TYPE, SIZE> &rhs) const {
		bool ret = true;
		for (auto i = 0; i < SIZE; i++) {
			ret &= (rhs.raw[i] == this->raw[i]);
		}
		return ret;
	}

	float length() const {
		float ret = 0;
		for (auto i = 0; i < SIZE; i++) {
			ret += this->raw[i] * this->raw[i];
		}
		return sqrtf(ret);

	}

	float distance(const TVecBase<TYPE, SIZE> &rhs) const {
		float ret = 0;
		float val;
		for (auto i = 0; i < SIZE; i++) {
			val = rhs.raw[i] - this->raw[i];
			ret += val * val;
		}
		return sqrtf(ret);
	}

	TYPE manhattanDistance(const TVecBase<TYPE, SIZE> &rhs) const {
		TYPE ret = 0;
		for (auto i = 0; i < SIZE; i++) {
			ret += std::abs(rhs.raw[i] - this->raw[i]);
		}
		return ret;
	}
};

template <typename TYPE, unsigned SIZE>
struct TVec : public TVecBase<TYPE, SIZE> {
	typedef TVecBase<TYPE, SIZE> BASE;
	using BASE::BASE;

	TVec<TYPE, SIZE> normalize() {
		TVec<TYPE, SIZE> ret;
		TYPE len = sqrtf(dot(*this));

		for (auto i = 0; i < SIZE; i++) {
			ret.raw[i] = this->raw[i] / len;
		}

		return ret;
	}

	TYPE dot(const TVec<TYPE, SIZE> &rhs) {
		TYPE accum = 0;
		for (auto i = 0; i < SIZE; i++) {
			accum += this->raw[i] * rhs.raw[i];
		}

		return accum;
	}

	template<unsigned S>
	operator TVec<TYPE, S>() {
		TVec<TYPE, S> ret;

		for (int i = 0; i < std::min(S, SIZE); i++) {
			ret.raw[i] = this->raw[i];
		}
		return ret;
	}
};

typedef TVec<float, 2> Vec2f;
typedef TVec<float, 3> Vec3f;
typedef TVec<float, 4> Vec4f;

template <typename TYPE, unsigned SIZE>
struct TPoint : public TVecBase<TYPE, SIZE> {
	typedef TVecBase<TYPE, SIZE> BASE;
	using BASE::BASE;

	template<unsigned S>
	operator TPoint<TYPE, S>() const {
		TPoint<TYPE, S> ret;

		for (int i = 0; i < std::min(S, SIZE); i++) {
			ret.raw[i] = this->raw[i];
		}
		return ret;
	}
};

typedef TPoint<float, 2> Point2f;
typedef TPoint<float, 3> Point3f;
typedef TPoint<float, 4> Point4f;

typedef TPoint<int, 2> Point2i;
typedef TPoint<int, 3> Point3i;
typedef TPoint<int, 4> Point4i;

template <typename TYPE>
struct TRect {
	TYPE x = 0, y = 0, width = 0, height = 0;

	TRect() {}
	TRect(const TYPE x, const TYPE y, const TYPE width, const TYPE height)
		: x(x), y(y), width(width), height(height) {}

	TPoint<TYPE, 2> randomPoint();
};

template <typename TYPE>
struct TCuboid {
	TYPE x = 0, y = 0, z = 0;
	TYPE width = 0, height = 0, depth = 0;

	TCuboid() {}
	TCuboid(const TYPE x, const TYPE y, const TYPE z, const TYPE width = 0, const TYPE height = 0, const TYPE depth = 0)
		: x(x), y(y), z(z), width(width), height(height), depth(depth) {}

	void includePoint(const TPoint<TYPE, 3> pt);

	bool containsPoint(const TPoint<TYPE, 3> pt);
	TPoint<TYPE, 3> boundPoint(const TPoint<TYPE, 3> &pt, TVec<TYPE, 3> *vec = nullptr);
	TPoint<TYPE, 3> clampPoint(const TPoint<TYPE, 3> &pt);

	TPoint<TYPE, 3> randomPoint();

	TCuboid<TYPE> inset(int16_t amount) {
		auto a2 = amount * 2;
		return TCuboid<TYPE>(x + amount, y + amount, z + amount, width - a2, height - a2, depth - a2);
	}
};

template<unsigned SIZE = 2>
class DynamicPoint {
public:
	bool active = false;

	TPoint<float, SIZE> origin;
	TVec<float, SIZE> vel;
	TVec<float, SIZE> acc;
	float friction = 0;

	DynamicPoint() {}
	DynamicPoint(TPoint<float, SIZE> origin, TVec<float, SIZE> vel, TVec<float, SIZE> acc, float friction = 0) :
		origin(origin), vel(vel), acc(acc), friction(friction) {}

	virtual void update() {
		origin += vel;
		vel += acc;

		if (friction != 0) {
			vel *= 1.0 - friction;
			acc *= 1.0 - friction;
		}
	}

	virtual void init() {}
};

template<unsigned SIZE = 2, typename POINT_TYPE = DynamicPoint<2>>
struct PointSystem {
	typedef typename std::conditional<SIZE == 3, TCuboid<float>, TRect<float>>::type BoundingType;

	std::vector<POINT_TYPE> points;
	TCuboid<float> bounds;

	bool bounded = false;
	uint16_t minPoints = 0;
	uint32_t pointTimeout = 0;
	uint64_t nextPointTime = 0;

	std::function<void(POINT_TYPE&)> afterInit = nullptr;

	PointSystem(size_t maxSize);

	void stopAll();
	void setMinPoints(uint16_t val) { minPoints = val; }
	void setMaxPoints(size_t size, bool enableNewPoints = false);
	void setVelocity(TVec<float, SIZE> vel);
	void setAddPointTimeout(uint32_t t) { pointTimeout = t; }
	void setAfterInit(std::function<void(POINT_TYPE&)> func) { afterInit = func; }

	void addPoints(int16_t count = 1);
	uint32_t activePointCount();

	void setBounds(BoundingType bounds);
	void addBoundedPoints(size_t pointCount, float velocity = 1.0);
	void update();
};

template <typename TYPE, unsigned SIZE>
struct TIndexedPoint : public TPoint<TYPE, SIZE> {
	uint32_t index = 0;

	template <typename T, unsigned S>
	TIndexedPoint(uint32_t index, TPoint<T, S> val) : TPoint<TYPE, SIZE>(val), index(index) {}
};

template <typename TYPE>
class TPointMapping {
public:
	std::vector<TIndexedPoint<TYPE, 3>> points;

	uint32_t minIndex = 0;
	uint32_t maxIndex = 0;

	TCuboid<TYPE> bounds;

	template <typename T, unsigned S>
	void addPoint(const TPoint<T, S> &point, uint32_t index) {
		if (points.size() == 0) {
			minIndex = maxIndex = index;
		}

		if (index < minIndex) minIndex = index;
		if (index > maxIndex) maxIndex = index;

		bounds.includePoint(point);
		points.push_back(TIndexedPoint<TYPE, 3>(index, point));
	}
	void clear() {
		minIndex = 0;
		maxIndex = 0;
		points.clear();
		bounds = TCuboid<TYPE>();
	}

	TPointMapping() {}
};

#include "geometry.tpp"

};
