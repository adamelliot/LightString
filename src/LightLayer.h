#pragma once

#include "utils.h"
#include "types.h"
#include "LightPattern.h"

namespace LightString {

template <typename FORMAT>
class LightLayer : public ILightLayer {
private:
	uint8_t layerID;

	ILightSection *section;

	std::vector<ILightPattern *> lightPatterns;
	std::vector<PatternCode> patternList;

	uint8_t patternIndex; // Index in the pattern order

	// Pattern Manager Timing
	// 0 -> let the pattern choose it's own timing
	// If the pattern specifies a time and this is set, the pattern will take precedent
	uint32_t maxPatternLength;
	uint32_t lastTime;
	uint32_t patternStartedAt;
	uint32_t pauseStartedAt;

	uint32_t transitionLength;
	uint32_t transitionStartedAt;
	bool runningBeginTransition;

	FORMAT opacity;

	ILightPattern *activePattern;

	PatternEvent patternEventHandler;

	EPlayState playState;
	EPlayMode playMode;
	ETransitionState transitionState;

	ILightPattern *getPattern(PatternCode patternCode);
	void updatePatternIndex(PatternCode patternCode);
	void finishPattern();

	void setPlayState(EPlayState playState);

	inline FORMAT getElapsedTimeRatio();

public:

	// Max Patterns is the upper limit for patterns, this only really applies to ARDUINO
	inline LightLayer(uint8_t maxPatterns = 5, uint8_t maxModes = 4) :
		patternIndex(0), maxPatternLength(0),
		lastTime(0), patternStartedAt(0), pauseStartedAt(0), transitionLength(kDefaultTransitionLength),
		transitionStartedAt(0), opacity(getMaxOpacity()), activePattern(0),
		patternEventHandler(0), playState(PATTERN_STOPPED), playMode(PLAY_MODE_CONTINUOUS),
		transitionState(TRANSITION_DONE) {}

	inline FORMAT getMaxOpacity();
	bool isActive() { return playState != PATTERN_STOPPED; }

	void setLayerID(uint8_t layerID) { this->layerID = layerID; }
	uint8_t getLayerID() { return layerID; }

	void setLightSection(ILightSection *section) { this->section = section; }
	ILightSection *getLightSection() { return section; }
	ILightPattern *getActivePattern() { return activePattern; }

	inline uint8_t getOpacity() { return opacity; }
	void setPalette(IPalette *palette) { if (activePattern) activePattern->setPalette(palette); }

	void setMaxPatternLength(uint32_t maxPatternLength) { this->maxPatternLength = maxPatternLength; }
	uint32_t getMaxPatternLength() { return maxPatternLength; }

	void setPatternEventHandler(PatternEvent patternEventHandler) { this->patternEventHandler = patternEventHandler; }
	PatternEvent getPatternEventHandler() { return patternEventHandler; }

	void setPlayMode(EPlayMode playMode) { this->playMode = playMode; }
	uint32_t getPlayMode() { return playMode; }

	// Play control
	void play(); // Starts from a stopped state, or unpauses.
	void stop();
	void pause();
	void unpause();

	bool startPattern(PatternCode patternCode);
	bool startRandomPattern();
	bool nextPattern();
	bool prevPattern();

	void shufflePatterns();

	void addLightPattern(ILightPattern &pattern, uint64_t modeList);
	void addLightPattern(ILightPattern &pattern);

	void updateTransition(uint32_t timeDelta);
	void update();
};

#include "LightLayer.tpp"

};
