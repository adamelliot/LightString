#pragma once

#include "utils.h"
#include "types.h"
#include "LightPattern.h"

namespace LightString {

template <typename FORMAT>
class LightLayer : public ILightLayer {
private:
	LightLayerConfig config;

	uint8_t layerID;

	ILightSection *section;

	std::vector<ILightPattern *> lightPatterns;
	std::vector<PatternCode> patternList;

	uint8_t patternIndex; // Index in the pattern order

	uint32_t lastTime;
	uint32_t patternStartedAt;
	uint32_t pauseStartedAt;

	uint32_t transitionStartedAt;
	bool runningBeginTransition;

	FORMAT opacity;

	ILightPattern *activePattern;

	// PatternEvent patternEventHandler;

	EPlayState playState;
	// EPlayMode playMode;
	ETransitionState transitionState;

	ILightPattern *getPattern(PatternCode patternCode);
	void updatePatternIndex(PatternCode patternCode);
	void finishPattern();

	void setPlayState(EPlayState playState);

	inline FORMAT getElapsedTimeRatio();

public:

	inline LightLayer() :
		patternIndex(0),
		lastTime(0), patternStartedAt(0), pauseStartedAt(0),
		transitionStartedAt(0), opacity(getMaxOpacity()), activePattern(0),
		playState(PATTERN_STOPPED), transitionState(TRANSITION_DONE) {}

	inline FORMAT getMaxOpacity();
	bool isActive() { return playState != PATTERN_STOPPED; }

	void setLayerID(uint8_t layerID) { this->layerID = layerID; }
	uint8_t getLayerID() { return layerID; }

	void setConfig(LightLayerConfig &config) { this->config = config; }
	LightLayerConfig &getConfig() { return config; }

	void setLightSection(ILightSection *section) { this->section = section; }
	ILightSection *getLightSection() { return section; }
	ILightPattern *getActivePattern() { return activePattern; }

	inline FORMAT getOpacity() { return opacity; }
	void setPalette(IPalette *palette) { if (activePattern) activePattern->setPalette(palette); }

	void setMaxPatternDuration(uint32_t maxPatternDuration) { this->config.maxPatternDuration = maxPatternDuration; }
	uint32_t getMaxPatternDuration() { return config.maxPatternDuration; }

	void setPatternEventHandler(PatternEvent patternEventHandler) { this->config.patternEventHandler = patternEventHandler; }
	PatternEvent getPatternEventHandler() { return config.patternEventHandler; }

	void setPlayMode(EPlayMode playMode) { this->config.playMode = playMode; }
	uint32_t getPlayMode() { return config.playMode; }

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
