#pragma once

#include <vector>
#include <memory>

#include "utils.h"
#include "types.h"
#include "LightLayer.h"

namespace LightString {

using std::vector;
using std::shared_ptr;

template <template <typename> class PIXEL, typename FORMAT, template <typename> class OUTPUT_PIXEL = TRGB>
class LightSection : public ILightSection {
private:
	FORMAT brightness;
	// Fade is used for transitioning brightness
	FORMAT fade, targetFade;
	uint32_t fadeEnd = 0;
	int32_t fadeDuration = kDefaultFadeOutDuration;

	void updateFadeLevel();

	typedef vector<shared_ptr<LightLayer<PIXEL, FORMAT>>> LayerList;
	LayerList m_layers;

protected:
	PatternProvider &patternProvider;
	PixelBufferProvider<PIXEL, FORMAT> m_bufferProvider;
	TPixelBuffer<OUTPUT_PIXEL, FORMAT> outputBuffer;

	bool pauseAfterFade = false;

public:

	LightSection(PatternProvider &patternProvider, int size) :
		brightness(TColorFormatHelper<FORMAT>::getMaxValue()),
		fade(TColorFormatHelper<FORMAT>::getMaxValue()),
		targetFade(TColorFormatHelper<FORMAT>::getMaxValue()),
		patternProvider(patternProvider),
		m_bufferProvider(size)
	{
		outputBuffer.resize(size);
	}

	virtual ~LightSection() {}

	PixelBufferProvider<PIXEL, FORMAT> &bufferProvider() { return m_bufferProvider; }
	void resize(int size);

	LayerList &layers() { return m_layers; }

	PatternProvider &getPatternProvider() { return patternProvider; }
	PixelBufferProvider<PIXEL, FORMAT> &getBufferProvider() { return bufferProvider; }

	int32_t getFadeDuration() { return fadeDuration; }
	void setFadeDuration(int32_t val) { fadeDuration = val; }

	TPixelBuffer<OUTPUT_PIXEL, FORMAT> &getOutputBuffer() { return outputBuffer; }

	int getTotalLayers() { return m_layers.size(); }

	LightLayer<PIXEL, FORMAT> *getLayer(int layerIndex) {
		return layerIndex < m_layers.size() ? m_layers[layerIndex].get() : nullptr;
	}

	void clearLayers() { m_layers.clear(); }
	void ensureLayerExists(int layerIndex);
	int indexOfLayer(ILightLayer *layer);

	bool startPatternOnNewLayer(PatternCode patternCode, LightLayerConfig config);
	void stopAllLayers(bool fadeOut);
	void startOutTransitionOnAllLayers();
	void playAllLayers();

	void pruneStoppedLayers();

	// Update all the layers and then compact them together into the outputBuffer
	void update();

	virtual void pause(bool blackout = true, bool fade = true);
	virtual void unpause(bool fade = true);

	void fadeDown();
	void fadeUp(bool forceZero = false);

	void setBrightness(FORMAT brightness) { this->brightness = brightness; }
	FORMAT getBrightness() { return brightness; }

};

#include "LightSection.tpp"

};
