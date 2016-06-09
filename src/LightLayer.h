#pragma once

#include "utils.h"
#include "types.h"
#include "LightPattern.h"

namespace LightString {

LIGHT_LAYER_TEMPLATE
class LightLayer : public ILightLayer {
private:
	uint8_t layerID;

	// TODO: Implement with virtual interface to remove the MAX_LAYER dependence 
	ILightSection *section;

	ILightPattern *lightPatterns[MAX_LIGHT_PROGRAMS];
	uint8_t patternCount;

	PatternCode patternList[MAX_LIGHT_PROGRAMS * MAX_MODES];
	uint8_t patternListLength;
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
	
	uint8_t opacity;

	ILightPattern *activePattern;

	PatternEvent patternEventHandler;

	EPlayState playState;
	EPlayMode playMode;
	ETransitionState transitionState;

	ILightPattern *getPattern(PatternCode patternCode);
	void updatePatternIndex(PatternCode patternCode);
	void finishPattern();

	void setPlayState(EPlayState playState);

public:

	inline LightLayer()
		: patternCount(0), patternListLength(0), patternIndex(0), maxPatternLength(0),
		lastTime(0), patternStartedAt(0), pauseStartedAt(0), transitionLength(kDefaultTransitionLength),
		transitionStartedAt(0), opacity(255), activePattern(0),
		patternEventHandler(0), playState(PROGRAM_STOPPED), playMode(PLAY_MODE_CONTINUOUS),
		transitionState(TRANSITION_DONE) {}

	bool isActive() { return playState != PROGRAM_STOPPED; }

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
