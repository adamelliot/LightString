#pragma once

#include "types.h"
#include "LightPattern.h"
#include "LightLayer.h"

using namespace LightString;

namespace LightString {

template <template <typename> class PIXEL = TRGB, typename FORMAT = uint8_t>
class ThinPatternManager : public LightLayer<FORMAT>, public ILightSection {
private:

	TPixelBuffer<PIXEL, FORMAT> *outputBuffer = NULL;

	uint32_t lastTime = 0;
	uint16_t msPerFrame = 25;

public:

	ThinPatternManager() {
		this->setLightSection(this);
	}

	IPixelBuffer *getOutputBuffer() { return outputBuffer; }

	uint8_t getTotalLayers() { return 1; }
	ILightLayer *getLayer(uint8_t layerID) { return this; }
	void ensureLayerExists(uint8_t layerID) {}

	IPixelBuffer *lockBuffer() { return outputBuffer; }
	void unlockBuffer(IPixelBuffer *buffer) {}
	bool addBuffer(IPixelBuffer *buffer) {
		outputBuffer = (TPixelBuffer<PIXEL, FORMAT> *)buffer;
		return true;
	}

	void setBuffer(TPixelBuffer<PIXEL, FORMAT> *buffer) {
		outputBuffer = buffer;
	}

	void setMaxFPS(uint16_t targetFPS) { 
		msPerFrame = (targetFPS == 0) ? 0 : 1000 / targetFPS; 
	}

	bool update();

};

#include "ThinPatternManager.tpp"

};
