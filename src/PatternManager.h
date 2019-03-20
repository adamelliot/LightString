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

using std::vector;
using std::shared_ptr;

template <template <typename> class PIXEL = TRGB, typename FORMAT = uint8_t, template <typename> class OUTPUT_PIXEL = TRGB>
class PatternManager {
private:
	typedef vector<shared_ptr<LIGHT_SECTION_CLASS>> SectionList;
	SectionList m_sections;

	uint32_t lastTime = 0;
	uint16_t msPerFrame = 25;

	uint8_t brightness = 255;
	int16_t targetBrightness = 255, adjustedBrightness = 255;
	int16_t brightnessStep = 0;

	LightLayerConfig layerConfig;

	void ensureLayerIsSetup(LIGHT_SECTION_CLASS &section, int layerIndex);

protected:
	PatternProvider &patternProvider;

public:
	PatternManager(PatternProvider &patternProvider) : patternProvider(patternProvider)
		{ lastTime = millis(); }
	~PatternManager() { clearLightSections(); }

	SectionList &sections() { return m_sections; }
	shared_ptr<LIGHT_SECTION_CLASS> &section(int i) { return m_sections[i]; }

	LIGHT_SECTION_CLASS *getSection(uint8_t sectionID) { return getLightSection(sectionID); }
	LIGHT_SECTION_CLASS *getLightSection(uint8_t sectionID);

	void setLayerConfig(const LightLayerConfig &config);
	void setLayerConfig(const LightLayerConfig &config, int layerIndex, uint8_t sectionID = 0);

	void setPatternEventHandler(PatternEvent patternEventHandler, void *userData = nullptr);
	void setPatternEventHandler(PatternEvent patternEventHandler, void *userData, int layerIndex, uint8_t sectionID = 0);

	void setPatternDuration(uint32_t patternDuration);
	void setPatternDuration(uint32_t patternDuration, int layerIndex, uint8_t sectionID = 0);

	void setTransitionDuration(uint32_t transitionDuration);
	void setTransitionDuration(uint32_t transitionDuration, int layerIndex, uint8_t sectionID = 0);
	
	void setPlayMode(EPlayMode playMode);
	void setPlayMode(EPlayMode playMode, int layerIndex, uint8_t sectionID = 0);

	void setBrightness(FORMAT brightness);
	void setBrightness(FORMAT brightness, uint8_t sectionID);

	void setMaxFPS(uint16_t targetFPS) { 
		msPerFrame = (targetFPS == 0) ? 0 : 1000 / targetFPS; 
	}

	void pause(bool blackout = true, bool fade = true);
	void unpause();

	// TODO: Make start pattern interface more consistent
	bool startPattern(PatternCode patternCode, int layerIndex, uint8_t sectionID = 0);
	void startPattern(uint8_t patternID, int layerIndex = 0);

	// Only starts on active layers
	void startRandomPattern();
	void startRandomPatternOnAllLayers();
	void startRandomPattern(int layerIndex, uint8_t sectionID = 0);

	// Start the current pattern
	void play();
	void play(int layerIndex, uint8_t sectionID = 0);

	void nextPattern(bool transition = false);
	void nextPattern(bool transition, int layerIndex, uint8_t sectionID = 0);
	void prevPattern(bool transition = false);
	void prevPattern(bool transition, int layerIndex, uint8_t sectionID = 0);

	void shufflePatterns();

	void setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex, uint32_t flags = RETAIN_PATTERN | FADE_OUT | TRANSITION_PATTERN);
	void setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex, int layerIndex, uint8_t sectionID = 0, uint32_t flags = RETAIN_PATTERN | FADE_OUT | TRANSITION_PATTERN);

	void clearPatternSequence(bool fadeOut = true);
	void clearPatternSequence(int layerIndex, uint8_t sectionID = 0, bool fadeOut = true);

	void addLightPattern(pattern_id_t patternID, int layerIndex = 0);
	void addLightPattern(pattern_id_t patternID, uint64_t modeList, int layerIndex);
	void addLightPattern(pattern_id_t patternID, uint64_t modeList, int layerIndex, uint8_t sectionID);

	void clearLightSections();
	uint8_t addLightSection(int size);
	uint8_t getTotalSections() { return m_sections.size(); }

	bool update();
};

#include "PatternManager.tpp"

};

#endif
