#pragma once

#include <unordered_map>

namespace std
{

template<>
struct hash<typename LightString::Point3i>
{
    std::size_t operator()(LightString::Point3i const& s) const
    {
    	uint8_t shift = sizeof(size_t) / 3;

        std::size_t const h1((uint32_t)s.x);
        std::size_t const h2((uint32_t)s.y);
        std::size_t const h3((uint32_t)s.z);

        return (h3 << (shift * 2)) | (h2 << shift) | h1;
    }
};

};

namespace LightString {

class PointMapping {
public:
	std::unordered_map<Point3i, uint32_t> points;

	uint32_t minIndex = 0xffffffff;
	uint32_t maxIndex = 0;

	TCuboid<int32_t> bounds;

	void addPoint(const Point3i &point, uint32_t index) {
		if (index < minIndex) minIndex = index;
		if (index > maxIndex) maxIndex = index;

		bounds.includePoint(point);

		points.emplace(point, index);
	}

	PointMapping() {}

	PointMapping(const PointMapping& mapping) :
		points(mapping.points), minIndex(mapping.minIndex), maxIndex(mapping.maxIndex) {}

};

template <template <typename> class T, typename FORMAT = uint8_t>
class TMappingPixelBuffer : public TMappingPixelBuffer3d<T, FORMAT> {
private:
	PointMapping mapping;

public:

	TPoint<FORMAT, 3> origin;

	TMappingPixelBuffer() : LightString::TMappingPixelBuffer3d<T, FORMAT>(1) {}

	TMappingPixelBuffer(const PointMapping &mapping)
		: LightString::TMappingPixelBuffer3d<T, FORMAT>(mapping.maxIndex) {
		setMapping(mapping);
	}

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

	virtual int16_t xy(int16_t x, int16_t y) {
		Point3i pt(x, y, 0);
		auto key = mapping.points.find(pt);
		if (key == mapping.points.end()) {
			return -1;
		} else {
			return (int16_t)key->second;
		}
	}

	virtual int16_t xyz(int16_t x, int16_t y, int16_t z) {
		Point3i pt(x, y, z);
		auto key = mapping.points.find(pt);
		if (key == mapping.points.end()) {
			return -1;
		} else {
			return (int16_t)key->second;
		}
	}
};

};
