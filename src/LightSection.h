#pragma once

#include "utils.h"
#include "types.h"
#include "LightLayer.h"

namespace LightString {

LIGHT_SECTION_TEMPLATE
class LightSection : public ILightSection {
private:
	
	IPixelBuffer *bufferPool[MAX_LAYERS];
	uint8_t activeBuffers;
	uint8_t bufferCount;

public:

	TPixelBuffer<PIXEL, FORMAT> *outputBuffer;
	LIGHT_LAYER_CLASS layers[MAX_LAYERS];

	inline LightSection()
		: activeBuffers(0), bufferCount(0), outputBuffer(0)
	{
		for (uint32_t i = 0; i < MAX_LAYERS; i++) {
			layers[i].setLayerID(i);
			layers[i].setLightSection(this);
		}
	}

	TPixelBuffer<PIXEL, FORMAT> *getOutputBuffer() { return outputBuffer; }

	uint8_t getMaxLayers() { return MAX_LAYERS; }
	ILightLayer *getLayer(uint8_t layerID) { return &layers[layerID]; }

	IPixelBuffer *lockBuffer();
	void unlockBuffer(IPixelBuffer *buffer);
	bool addBuffer(IPixelBuffer *buffer);

	// Update all the layers and then compact them together into the outputBuffer
	void update();
};

#include "LightSection.tpp"

};
