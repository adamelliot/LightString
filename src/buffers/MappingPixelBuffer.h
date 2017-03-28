#pragma once

#include <unordered_map>

namespace LightString {

template <template <typename> class T, typename FORMAT = uint8_t>
class TMappingPixelBuffer : public TPixelBuffer<T, FORMAT> {
protected:
	TPointMapping<float> mapping;

public:
	TPoint<FORMAT, 3> origin;
	float width, height, depth;

	TMappingPixelBuffer() : LightString::TPixelBuffer<T, FORMAT>(1) {}

	TMappingPixelBuffer(const TPointMapping<float> &mapping)
		: TPixelBuffer<T, FORMAT>(mapping.maxIndex) {
		setMapping(mapping);
	}

	const TPointMapping<float> &getMapping() const { return mapping; }
	const std::vector<TIndexedPoint<float, 3>> &getPoints() const { return mapping.points; }

	void setMapping(const TPointMapping<float> &mapping) {
		this->resize(mapping.maxIndex + 1);
		this->mapping = mapping;

		this->width  = mapping.bounds.width + 1;
		this->height = mapping.bounds.height + 1;
		this->depth  = mapping.bounds.depth + 1;

		origin.x = mapping.bounds.x;
		origin.y = mapping.bounds.y;
		origin.z = mapping.bounds.z;
	}
/*
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
	}*/
};

};
