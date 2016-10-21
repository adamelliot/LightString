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

	// If pattern sequence is set play mode will use the sequence
	// otherwise standard play back semantics will be observed.
	//
	// Note:
	// - If the layer is sequenced randomizing has no effect
	// - Settings from cue can still be over ridden by the pattern methods:
	//		- isPatternFinished, getNextPatternCode, getInTransition, getOutTransition
	PatternSequence *patternSequence = nullptr;

	// Cloning patterns is used when patterns could run in parallel and should
	// be controlled by the wrapping section and pattern manager
	bool clonePatterns = false;

	uint8_t patternIndex = 0; // Index in the pattern order or the sequence

	uint32_t lastTime = 0;
	uint32_t patternStartedAt = 0;
	uint32_t pauseStartedAt = 0;

	uint32_t transitionStartedAt = 0;
	bool runningBeginTransition = false;

	FORMAT opacity = getMaxOpacity();

	ILightPattern *activePattern = nullptr;

	EPlayState playState = PATTERN_STOPPED;
	ETransitionState transitionState = TRANSITION_DONE;
	EPatternTransition currentTransition = TRANSITION_DEFAULT;

	ILightPattern *getPattern(PatternCode patternCode);
	void updatePatternIndex(PatternCode patternCode);
	void finishPattern();

	void setPlayState(EPlayState playState);

	inline FORMAT getElapsedTimeRatio();

	void startPattern(ILightPattern *pattern, uint8_t mode, PatternConfig *config = nullptr);
	bool startSelectedPattern();

public:

	inline LightLayer() {}
	~LightLayer();

	inline FORMAT getMaxOpacity();
	bool isActive() { return playState != PATTERN_STOPPED; }

	void setLayerID(uint8_t layerID) { this->layerID = layerID; }
	uint8_t getLayerID() { return layerID; }

	void setConfig(LightLayerConfig &config) { this->config = config; }
	LightLayerConfig &getConfig() { return config; }

	void setLightSection(ILightSection *section) { this->section = section; }
	ILightSection *getLightSection() { return section; }
	ILightPattern *getActivePattern() { return activePattern; }

	uint8_t getPatternIndex() const { return patternIndex; }

	inline FORMAT getOpacity() { return opacity; }
	void setPalette(IPalette *palette) { if (activePattern) activePattern->setPalette(palette); }

	void setPatternSequence(const PatternSequence &patternSequence);

	EPatternTransition getSelectedInTransition();
	EPatternTransition getSelectedOutTransition();
	int32_t getSelectedPatternDuration();
	int32_t getSelectedTransitionDuration();

	void setTransitionDuration(int32_t transitionDuration) { this->config.transitionDuration = transitionDuration; }

	void setPatternDuration(int32_t patternDuration) { this->config.patternDuration = patternDuration; }
	uint32_t getPatternDuration() { return config.patternDuration; }

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
