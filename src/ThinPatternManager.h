#pragma once

#include "types.h"
#include "LightPattern.h"
#include "LightLayer.h"
#include "LightSection.h"

using namespace LightString;

namespace LightString {

template <template <typename> class PIXEL = TRGB, typename FORMAT = uint8_t>
class ThinPatternManager : public LightSection<PIXEL, FORMAT, PIXEL>, public LightLayer<PIXEL, FORMAT> {
private:
	uint32_t lastTime = 0;
	uint16_t msPerFrame = 25;

public:
	ThinPatternManager(PatternProvider &provider, int size) :
		LightSection<PIXEL, FORMAT, PIXEL>(provider, size),
		LightLayer<PIXEL, FORMAT>(provider, static_cast<LightSection<PIXEL, FORMAT, PIXEL> *>(this)->bufferProvider())
	{
		this->setLightSection(this);
	}

	int getTotalLayers() { return 1; }
	LightLayer<PIXEL, FORMAT> *getLayer(int layerIndex) { return this; }
	void ensureLayerExists(int layerIndex) {}
	int indexOfLayer(ILightLayer *) { return 0; }

	void setMaxFPS(uint16_t targetFPS) {
		msPerFrame = (targetFPS == 0) ? 0 : 1000 / targetFPS;
	}

	virtual void pause() {
		pause(true, true);
	}

	virtual void pause(bool blackout, bool fade = true) {
		if (blackout) {
			if (fade) {
				this->fadeDown();
				this->pauseAfterFade = true;
			} else {
				this->setBrightness(0);
				LightLayer<PIXEL, FORMAT>::pause();
			}
		} else {
			if (!fade) {
				LightLayer<PIXEL, FORMAT>::pause();
			}
		}
	}

	void unpause(bool _) {
		LightLayer<PIXEL, FORMAT>::unpause();
	}

	void unpause() {
		LightLayer<PIXEL, FORMAT>::unpause();
	}


	bool update();

};

#include "ThinPatternManager.tpp"

};
