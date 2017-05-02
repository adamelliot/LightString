#pragma once

#include <unordered_map>

namespace LightString {

template <template <typename> class T, typename FORMAT = uint8_t>
class TMappingPixelBuffer : public TPixelBufferAdapter<T, FORMAT> {
protected:
	using TPixelBufferAdapter<T, FORMAT>::buffer;
	TPointMapping<float> mapping;

public:
	TPoint<float, 3> origin;
	float width = 0, height = 0, depth = 0;

	TMappingPixelBuffer() {}

	TMappingPixelBuffer(TPixelBuffer<T, FORMAT> *buffer) : TPixelBufferAdapter<T, FORMAT>(buffer) {}

	TMappingPixelBuffer(TPixelBuffer<T, FORMAT> *buffer, const TPointMapping<float> &mapping)
		: TPixelBufferAdapter<T, FORMAT>(buffer) {
		setMapping(mapping);
	}

	virtual ~TMappingPixelBuffer() {}

	/* ------------ Mapping Methods -------------- */

	const TPointMapping<float> &getMapping() const { return mapping; }
	const std::vector<TIndexedPoint<float, 3>> &getPoints() const { return mapping.points; }

	uint32_t getRequiredBufferSize() { return mapping.maxIndex + 1; }

	void setMapping(const TPointMapping<float> &mapping) {
		if (buffer) buffer->resize(mapping.maxIndex + 1);
		this->mapping = mapping;

		this->width  = mapping.bounds.width;
		this->height = mapping.bounds.height;
		this->depth  = mapping.bounds.depth;

		origin.x = mapping.bounds.x;
		origin.y = mapping.bounds.y;
		origin.z = mapping.bounds.z;
	}

};

};
