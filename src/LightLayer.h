#pragma once

#include <memory>

#include "utils.h"
#include "types.h"
#include "LightPattern.h"

namespace LightString {

template <typename FORMAT>
class LightLayer : public ILightLayer {
private:
	LightLayerConfig config;

	uint8_t layerID = 0;

	ILightSection *section = nullptr;

	PatternProvider &patternProvider;
	std::vector<PatternCode> patternList;

	// next / previous / random and loading by index require a patternSequence
	// Note:
	// - Settings from cue can still be over ridden by the pattern methods:
	//		- isPatternFinished, getNextPatternCode, getInTransition, getOutTransition
	std::shared_ptr<IPatternSequence> patternSequence { nullptr };

	int patternIndex = 0; // Index in the pattern order or the sequence

	bool holdPattern = false;

	bool patternIsFinished = false;

	bool skipInTransition = false;
	bool skipOutTransition = false;

	uint32_t lastTime = 0;
	uint32_t patternStartedAt = 0;
	uint32_t pauseStartedAt = 0;

	uint32_t transitionStartedAt = 0;
	bool runningBeginTransition = false;

	PatternCode enqueuedPattern;
	int enqueuedPatternIndex = 0;

	// How the pattern behaves after it finishes
	EPlayOutAction playOutAction = LOAD_NEXT;

	FORMAT opacity = getMaxOpacity();
	FORMAT transitionOpacity = getMaxOpacity();

	ILightPattern *activePattern = nullptr;

	EPlayState playState = PATTERN_STOPPED;
	ETransitionState transitionState = TRANSITION_DONE;
	EPatternTransition currentTransition = TRANSITION_DEFAULT;

	// Sets the patterns time to be past the end of it's playback
	void setPatternIsFinished() { patternIsFinished = true; }

	// void updatePatternIndex(PatternCode patternCode);
	void finishPattern();

	void setPlayState(EPlayState playState);
	void triggerShowEvent(EPlayState showState);

	FORMAT getElapsedTimeRatio(int32_t transitionDuration);

	void startPattern(ILightPattern *pattern, uint8_t mode, PatternConfig *config = nullptr);
	bool startSelectedPattern();

public:

	LightLayer(PatternProvider &patternProvider) : patternProvider(patternProvider) {}
	virtual ~LightLayer() {
		finishPattern();
	}

	inline FORMAT getMaxOpacity();
	EPlayState getPlayState() { return playState; }
	bool isActive() { return playState != PATTERN_STOPPED; }

	bool isStopped() { return playState == PATTERN_STOPPED; }
	bool willStop() { return (playOutAction == FADE_TO_STOP || playOutAction == FREEZE_FADE_TO_STOP) && patternIsFinished; }
	bool isPaused() { return playState == PATTERN_PAUSED; }

	bool isPatternHeld() const { return holdPattern; }

	void setLayerID(uint8_t layerID) { this->layerID = layerID; }
	uint8_t getLayerID() { return layerID; }

	bool isRunningPatternFromSequence() { return patternSequence && patternSequence->size() > 0; }
	int getPatternIndex() { return patternIndex; }
	uint32_t getElapsedTime() {
		if (playState == PATTERN_STOPPED) return 0;

		uint32_t offset = ((pauseStartedAt != 0) ? millis() - pauseStartedAt : 0);
		return (patternStartedAt == 0) ? 0 :
			(millis() - patternStartedAt - offset);
	}
	uint32_t getTransitionTimeElapsed() { 
		if (playState == PATTERN_STOPPED) return 0;

		uint32_t offset = ((pauseStartedAt != 0) ? millis() - pauseStartedAt : 0);
		return millis() - transitionStartedAt - offset;
	}

	void setConfig(const LightLayerConfig &config) { this->config = config; }
	LightLayerConfig &getConfig() { return config; }

	void setLightSection(ILightSection *section) { this->section = section; }
	ILightSection *getLightSection() { return section; }
	ILightPattern *getActivePattern() { return activePattern; }

	int getPatternIndex() const { return patternIndex; }

	PatternCode getPatternCodeFromIndex(uint8_t index);
	int getPlaybackCount() const;

	void setOpacity(FORMAT val) { opacity = val; }
	FORMAT getOpacity() { return opacity; }
	FORMAT getTransitionOpacity() { return transitionOpacity; }

	void setPalette(IPalette *palette) { if (activePattern) activePattern->setPalette(palette); }

	void setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex = -1, uint32_t flags = RETAIN_PATTERN | FADE_OUT | TRANSITION_PATTERN);
	void clearPatternSequence(bool fadeOut = true, bool stopIfPlayingFromSequence = true);

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
	void stop(bool fadeOut = true);
	virtual void pause();
	virtual void unpause();

	void hold();
	void unhold();

	void enqueuePattern(PatternCode patternCode, bool waitToFinish = false);
	bool startPattern(PatternCode patternCode, bool transition = false);

	bool enqueuePatternAtIndex(size_t index, bool waitToFinish = false);
	bool startPatternAtIndex(size_t index, bool transition = false);

	bool startRandomPattern(bool transition = false);
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
