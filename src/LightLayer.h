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

	PatternProvider &patternProvider;
	std::vector<PatternCode> patternList;

	// If pattern sequence is set play mode will use the sequence
	// otherwise standard play back semantics will be observed.
	//
	// Note:
	// - If the layer is sequenced randomizing has no effect
	// - Settings from cue can still be over ridden by the pattern methods:
	//		- isPatternFinished, getNextPatternCode, getInTransition, getOutTransition
	PatternSequence *patternSequence = nullptr;

	uint8_t patternIndex = 0; // Index in the pattern order or the sequence

	uint32_t lastTime = 0;
	uint32_t patternStartedAt = 0;
	uint32_t pauseStartedAt = 0;

	uint32_t transitionStartedAt = 0;
	bool runningBeginTransition = false;

	PatternCode enqueuedPattern;
	bool loadEnqueued = false;
	bool loadPrevious = false;

	FORMAT opacity = getMaxOpacity();
	FORMAT transitionOpacity = getMaxOpacity();

	ILightPattern *activePattern = nullptr;

	EPlayState playState = PATTERN_STOPPED;
	ETransitionState transitionState = TRANSITION_DONE;
	EPatternTransition currentTransition = TRANSITION_DEFAULT;

	void updatePatternIndex(PatternCode patternCode);
	void finishPattern();

	void setPlayState(EPlayState playState);

	FORMAT getElapsedTimeRatio(int32_t transitionDuration);

	void startPattern(ILightPattern *pattern, uint8_t mode, PatternConfig *config = nullptr);
	bool startSelectedPattern();

public:

	LightLayer(PatternProvider &patternProvider) : patternProvider(patternProvider) {}
	~LightLayer();

	inline FORMAT getMaxOpacity();
	EPlayState getPlayState() { return playState; }
	bool isActive() { return playState != PATTERN_STOPPED; }

	void setLayerID(uint8_t layerID) { this->layerID = layerID; }
	uint8_t getLayerID() { return layerID; }

	uint8_t getPatternIndex() { return patternIndex; }
	uint32_t getElapsedTime() { return (patternStartedAt == 0) ? 0 : millis() - patternStartedAt; }
	uint32_t getTransitionTimeElapsed() { return millis() - transitionStartedAt; }

	void setConfig(const LightLayerConfig &config) { this->config = config; }
	LightLayerConfig &getConfig() { return config; }

	void setLightSection(ILightSection *section) { this->section = section; }
	ILightSection *getLightSection() { return section; }
	ILightPattern *getActivePattern() { return activePattern; }

	uint8_t getPatternIndex() const { return patternIndex; }
	PatternCode getPatternCodeFromIndex(uint8_t index);
	int getPlaybackCount() const;

	inline void setOpacity(FORMAT val) { opacity = val; }
	inline FORMAT getOpacity() { return opacity; }
	inline FORMAT getTransitionOpacity() { return transitionOpacity; }

	void setPalette(IPalette *palette) { if (activePattern) activePattern->setPalette(palette); }

	void setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex = 0, bool restartPattern = true);
	void clearPatternSequence();

	EPatternTransition getSelectedInTransition();
	EPatternTransition getSelectedOutTransition();
	int32_t getSelectedInTransitionDuration();
	int32_t getSelectedOutTransitionDuration();
	int32_t getSelectedPatternDuration();

	void setTransitionDuration(int32_t transitionDuration) {
		this->config.inTransitionDuration = transitionDuration;
		this->config.outTransitionDuration = transitionDuration;
	}

	void setPatternDuration(int32_t patternDuration) { this->config.patternDuration = patternDuration; }
	uint32_t getPatternDuration() { return config.patternDuration; }

	void setPatternEventHandler(PatternEvent patternEventHandler, void *userData = nullptr);

	void setPlayMode(EPlayMode playMode) { this->config.playMode = playMode; }
	uint32_t getPlayMode() { return config.playMode; }

	// Play control
	void play(); // Starts from a stopped state, or unpauses.
	void stop();
	virtual void pause();
	virtual void unpause();

	bool enqueuePattern(PatternCode patternCode, bool waitToFinish = false);
	bool startPattern(PatternCode patternCode);
	bool startRandomPattern();
	bool nextPattern(bool transition = false);
	bool prevPattern(bool transition = false);

	void shufflePatterns();

	void addLightPattern(pattern_id_t patternID, uint64_t modeList);
	void addLightPattern(pattern_id_t patternID);

	void updateTransition(uint32_t timeDelta);
	void update();
};

#include "LightLayer.tpp"

};
