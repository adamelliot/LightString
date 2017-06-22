#pragma once

#ifndef _LIGHTPROGRAMMANAGER_H_
#define _LIGHTPROGRAMMANAGER_H_

#include <vector>
#include <memory>

#include "types.h"
#include "LightPattern.h"
#include "LightLayer.h"
#include "LightSection.h"

using namespace LightString;

namespace LightString {

template <template <typename> class PIXEL = TRGB, typename FORMAT = uint8_t, template <typename> class OUTPUT_PIXEL = TRGB>
class PatternManager {
private:
	std::vector<std::shared_ptr<LIGHT_SECTION_CLASS>> sections;

	uint32_t lastTime = 0;
	uint16_t msPerFrame = 25;

	uint8_t brightness = 255;
	int16_t targetBrightness = 255, adjustedBrightness = 255;
	int16_t brightnessStep = 0;

	LightLayerConfig layerConfig;

	void ensureLayerIsSetup(LIGHT_SECTION_CLASS &section, uint8_t layerID);

protected:
	PatternProvider &patternProvider;

public:
	PatternManager(PatternProvider &patternProvider) : patternProvider(patternProvider)
		{ lastTime = millis(); }

	LIGHT_SECTION_CLASS *getSection(uint8_t sectionID) { return getLightSection(sectionID); }
	LIGHT_SECTION_CLASS *getLightSection(uint8_t sectionID);

	void setLayerConfig(const LightLayerConfig &config);
	void setLayerConfig(const LightLayerConfig &config, uint8_t layerID, uint8_t sectionID = 0);

	void setPatternEventHandler(PatternEvent patternEventHandler, void *userData = nullptr);
	void setPatternEventHandler(PatternEvent patternEventHandler, void *userData, uint8_t layerID, uint8_t sectionID = 0);

	void setPatternDuration(uint32_t patternDuration);
	void setPatternDuration(uint32_t patternDuration, uint8_t layerID, uint8_t sectionID = 0);

	void setTransitionDuration(uint32_t transitionDuration);
	void setTransitionDuration(uint32_t transitionDuration, uint8_t layerID, uint8_t sectionID = 0);
	
	void setPlayMode(EPlayMode playMode);
	void setPlayMode(EPlayMode playMode, uint8_t layerID, uint8_t sectionID = 0);

	void setBrightness(FORMAT brightness);
	void setBrightness(FORMAT brightness, uint8_t sectionID);

	void setMaxFPS(uint16_t targetFPS) { 
		msPerFrame = (targetFPS == 0) ? 0 : 1000 / targetFPS; 
	}

	void pause(bool blackout = true, bool fade = true);
	void unpause();

	// TODO: Make start pattern interface more consistent
	bool startPattern(PatternCode patternCode, uint8_t layerID, uint8_t sectionID = 0);
	void startPattern(uint8_t patternID, uint8_t layerID = 0);

	// Only starts on active layers
	void startRandomPattern();
	void startRandomPatternOnAllLayers();
	void startRandomPattern(uint8_t layerID, uint8_t sectionID = 0);

	// Start the current pattern
	void play();
	void play(uint8_t layerID, uint8_t sectionID = 0);

	void nextPattern(bool transition = false);
	void nextPattern(bool transition, uint8_t layerID, uint8_t sectionID = 0);
	void prevPattern(bool transition = false);
	void prevPattern(bool transition, uint8_t layerID, uint8_t sectionID = 0);

	void shufflePatterns();

	void setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex = 0, bool restartPattern = true);
	void setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex, bool restartPattern, uint8_t layerID, uint8_t sectionID = 0);

	void clearPatternSequence();
	void clearPatternSequence(uint8_t layerID, uint8_t sectionID = 0);

	void addLightPattern(pattern_id_t patternID, uint8_t layerID = 0);
	void addLightPattern(pattern_id_t patternID, uint64_t modeList, uint8_t layerID);
	void addLightPattern(pattern_id_t patternID, uint64_t modeList, uint8_t layerID, uint8_t sectionID);

	uint8_t addLightSection(TPixelBuffer<OUTPUT_PIXEL, FORMAT> &pixelBuffer);
	uint8_t getTotalSections() { return sections.size(); }

	bool addBufferToLightSection(uint8_t sectionID, TPixelBuffer<PIXEL, FORMAT> &buffer);

	bool update();
};

#include "PatternManager.tpp"

};

#endif