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

	FORMAT brightness;
	// Fade is used for transitioning brightness
	FORMAT fade, targetFade;
	uint32_t fadeEnd = 0;
	int32_t fadeDuration = 1000;

	void updateFadeLevel();

protected:
	PatternProvider &patternProvider;
	bool pauseAfterFade = false;

public:

	LightSection(PatternProvider &patternProvider) :
		brightness(TColorFormatHelper<FORMAT>::getMaxValue()),
		fade(TColorFormatHelper<FORMAT>::getMaxValue()),
		targetFade(TColorFormatHelper<FORMAT>::getMaxValue()),
		patternProvider(patternProvider) {}

	TPixelBuffer<OUTPUT_PIXEL, FORMAT> *outputBuffer = nullptr;
	std::vector<LightLayer<FORMAT>> layers;

	PatternProvider &getPatternProvider() { return patternProvider; }

	int32_t getFadeDuration() { return fadeDuration; }
	void setFadeDuration(int32_t val) { fadeDuration = val; }

	IPixelBuffer *getOutputBuffer() { return outputBuffer; }

	uint8_t getTotalLayers() { return layers.size(); }
	ILightLayer *getLayer(uint8_t layerID) { return &layers[layerID]; }
	void ensureLayerExists(uint8_t layerID);

	IPixelBuffer *lockBuffer();
	void unlockBuffer(IPixelBuffer *buffer);
	bool addBuffer(IPixelBuffer *buffer);

	// Update all the layers and then compact them together into the outputBuffer
	void update();

	virtual void pause(bool blackout = true, bool fade = true);
	virtual void unpause();

	void fadeDown();
	void fadeUp(bool forceZero = true);

	void setBrightness(FORMAT brightness) { this->brightness = brightness; }
	FORMAT getBrightness() { return brightness; }

};

#include "LightSection.tpp"

};
