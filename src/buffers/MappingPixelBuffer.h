#pragma once

#include <unordered_map>

namespace std
{

template<>
struct hash<typename LightString::Point3f>
{
    std::size_t operator()(LightString::Point3f const& s) const
    {
    	const float scale = 100000;
    	union {
    		float f;
    		size_t val;
    	} u = { ((s.x * scale * scale) + (s.y * scale) + s.z) };
    	return u.val;//reinterpret_cast<size_t>((s.x * scale * scale) + (s.y * scale) + s.z);
    }
};

};

namespace LightString {

template <typename TYPE, unsigned SIZE>
struct TIndexedPoint : public TPoint<TYPE, SIZE> {
	uint32_t index = 0;

	TIndexedPoint(uint32_t index, TPoint<TYPE, SIZE> val) : index(index), TPoint<TYPE, SIZE>(val) {}
};

class PointMapping {
public:
	std::unordered_map<Point3f, uint32_t> mappedPoints;
	std::vector<TIndexedPoint<float, 3>> points;

	uint32_t minIndex = 0xffffffff;
	uint32_t maxIndex = 0;

	TCuboid<float> bounds;

	void addPoint(const Point3f &point, uint32_t index) {
		if (index < minIndex) minIndex = index;
		if (index > maxIndex) maxIndex = index;

		bounds.includePoint(point);

		points.push_back(TIndexedPoint<float, 3>(index, point));
		mappedPoints.emplace(point, index);
	}

	PointMapping() {}

	PointMapping(const PointMapping& mapping) :
		points(mapping.points), mappedPoints(mapping.mappedPoints),
		minIndex(mapping.minIndex), maxIndex(mapping.maxIndex) {}

};

template <template <typename> class T, typename FORMAT = uint8_t>
class TMappingPixelBuffer : public TMappingPixelBuffer3d<T, FORMAT> {
protected:
	PointMapping mapping;

public:

	TPoint<FORMAT, 3> origin;

	TMappingPixelBuffer() : LightString::TMappingPixelBuffer3d<T, FORMAT>(1) {}

	TMappingPixelBuffer(const PointMapping &mapping)
		: LightString::TMappingPixelBuffer3d<T, FORMAT>(mapping.maxIndex) {
		setMapping(mapping);
	}

	const std::vector<TIndexedPoint<float, 3>> &getPoints() { return mapping.points; }

	void setMapping(const PointMapping &mapping) {
		this->resize(mapping.maxIndex + 1);
		this->mapping = mapping;

		this->width  = mapping.bounds.width + 1;
		this->height = mapping.bounds.height + 1;
		this->depth  = mapping.bounds.depth + 1;

		origin.x = mapping.bounds.x;
		origin.y = mapping.bounds.y;
		origin.z = mapping.bounds.z;
	}

	// [[deprecated("Inverse mapping is inefficient and incompatible with full point clouds.")]]
	virtual int16_t xy(int16_t x, int16_t y) {
		Point3f pt(x, y, 0);
		auto key = mapping.mappedPoints.find(pt);
		if (key == mapping.mappedPoints.end()) {
			return -1;
		} else {
			return (int16_t)key->second;
		}
	}

	// [[deprecated("Inverse mapping is inefficient and incompatible with full point clouds.")]]
	virtual int16_t xyz(int16_t x, int16_t y, int16_t z) {
		Point3f pt(x, y, z);
		auto key = mapping.mappedPoints.find(pt);
		if (key == mapping.mappedPoints.end()) {
			return -1;
		} else {
			return (int16_t)key->second;
		}
		return -1;
	}
};

};
