#pragma once

#ifndef _LIGHTPROGRAMMANAGER_H_
#define _LIGHTPROGRAMMANAGER_H_

#include <vector>

#include "types.h"
#include "LightPattern.h"
#include "LightLayer.h"
#include "LightSection.h"

using namespace LightString;

namespace LightString {

template <template <typename> class PIXEL = TRGB, typename FORMAT = uint8_t, template <typename> class OUTPUT_PIXEL = TRGB>
class PatternManager {
private:
	std::vector<LIGHT_SECTION_CLASS> sections;

	uint32_t lastTime = 0;
	uint16_t msPerFrame = 25;

	uint8_t brightness = 255;
	int16_t targetBrightness = 255, adjustedBrightness = 255;
	int16_t brightnessStep = 0;

	LightLayerConfig layerConfig;

	void ensureLayerIsSetup(uint8_t sectionID, uint8_t layerID);

public:

	PatternManager()
		{ lastTime = millis(); }

	LIGHT_SECTION_CLASS *getLightSection(uint8_t sectionID);

	void setPatternEventHandler(PatternEvent patternEventHandler);
	void setPatternEventHandler(PatternEvent patternEventHandler, uint8_t layerID, uint8_t sectionID = 0);

	void setPatternDuration(uint32_t patternDuration);
	void setPatternDuration(uint32_t patternDuration, uint8_t layerID, uint8_t sectionID = 0);

	void setTransitionDuration(uint32_t transitionDuration);
	void setTransitionDuration(uint32_t transitionDuration, uint8_t layerID, uint8_t sectionID = 0);
	
	void setPlayMode(EPlayMode playMode);
	void setPlayMode(EPlayMode playMode, uint8_t layerID, uint8_t sectionID = 0);

	void setMaxFPS(uint16_t targetFPS) { 
		msPerFrame = (targetFPS == 0) ? 0 : 1000 / targetFPS; 
	}
	
	void pause(bool blackout = true, bool fade = true);
	void unpause();

	bool startPattern(PatternCode patternCode, uint8_t layerID, uint8_t sectionID = 0);
	bool startPattern(uint8_t patternID, uint8_t layerID = 0);

	// Only starts on active layers
	void startRandomPattern();
	void startRandomPatternOnAllLayers();
	void startRandomPattern(uint8_t layerID, uint8_t sectionID = 0);

	// Start the current pattern
	void play();
	void play(uint8_t layerID, uint8_t sectionID = 0);

	void nextPattern();
	void nextPattern(uint8_t layerID, uint8_t sectionID = 0);
	void prevPattern();
	void prevPattern(uint8_t layerID, uint8_t sectionID = 0);

	void shufflePatterns();

	void addLightPattern(ILightPattern &pattern, uint8_t layerID = 0);
	void addLightPattern(ILightPattern &pattern, uint64_t modeList, uint8_t layerID);
	void addLightPattern(ILightPattern &pattern, uint64_t modeList, uint8_t layerID, uint8_t sectionID);

	uint8_t addLightSection(TPixelBuffer<OUTPUT_PIXEL, FORMAT> &pixelBuffer);
	bool addBufferToLightSection(uint8_t sectionID, TPixelBuffer<PIXEL, FORMAT> &buffer);

	void fadeDown();
	void fadeUp(bool forceZero = true);
	void setBrightness(uint8_t brightness);
	bool isTransitioning();
	void transitionBrightness();

	bool update();
};

#include "PatternManager.tpp"

};

#endif