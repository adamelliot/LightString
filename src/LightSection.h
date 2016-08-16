#pragma once

#include <vector>

#include "utils.h"
#include "types.h"
#include "LightLayer.h"

namespace LightString {

template <template <typename> class PIXEL, typename FORMAT, template <typename> class OUTPUT_PIXEL = TRGB>
class LightSection : public ILightSection {
private:

	std::vector<IPixelBuffer *> bufferPool;
	uint8_t activeBuffers = 0;

public:

	TPixelBuffer<OUTPUT_PIXEL, FORMAT> *outputBuffer = NULL;
	std::vector<LightLayer<FORMAT>> layers;

	IPixelBuffer *getOutputBuffer() { return outputBuffer; }

	inline uint8_t getTotalLayers() { return layers.size(); }
	ILightLayer *getLayer(uint8_t layerID) { return &layers[layerID]; }
	void ensureLayerExists(uint8_t layerID);

	IPixelBuffer *lockBuffer();
	void unlockBuffer(IPixelBuffer *buffer);
	bool addBuffer(IPixelBuffer *buffer);

	// Update all the layers and then compact them together into the outputBuffer
	void update();
};

#include "LightSection.tpp"

};
