#pragma once

#include "utils.h"
#include "types.h"
#include "LightPattern.h"

namespace LightString {

/*! Used to describe the behavior after a pattern finishes and which pattern to load.
	If any of the actions fail the `LAYER` is put in a STOPPED state.
*/
typedef enum {
	LOAD_NEXT = 1, /*!< Next will load the next in the sequence.
		If the pattern specifies a specific next pattern through getNextPatternCode
		that will be loaded instead of the next in the sequence. */
	LOAD_PREVIOUS, /*!< Load the previous pattern in the sequence */

	LOAD_ENQUEUED_PATTERN, /*!< Load the patten code enqueued with enqueuePattern */
	LOAD_ENQUEUED_INDEX, /*!< Load the patten cue enqueued with enqueuePatternAtIndex */

	FADE_TO_STOP /*!< Cause the layer to fade out then `STOP` */
} EPlayOutAction;

template <typename FORMAT>
class LightLayer : public ILightLayer {
private:
	LightLayerConfig config;

	uint8_t layerID = 0;

	ILightSection *section = nullptr;

	PatternProvider &patternProvider;
	std::vector<PatternCode> patternList;

	// If pattern sequence is set play mode will use the sequence
	// otherwise standard play back semantics will be observed.
	//
	// Note:
	// - If the layer is sequenced randomizing has no effect
	// - Settings from cue can still be over ridden by the pattern methods:
	//		- isPatternFinished, getNextPatternCode, getInTransition, getOutTransition
	PatternSequence patternSequence;
	bool hasPatternSequence = false;

	int patternIndex = 0; // Index in the pattern order or the sequence

	bool patternIsFinished = false;

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

	// bool runningPatternFromSequence = false;

	ILightPattern *activePattern = nullptr;

	EPlayState playState = PATTERN_STOPPED;
	ETransitionState transitionState = TRANSITION_DONE;
	EPatternTransition currentTransition = TRANSITION_DEFAULT;

	// Sets the patterns time to be past the end of it's playback
	void setPatternIsFinished() { patternIsFinished = true; }

	// void updatePatternIndex(PatternCode patternCode);
	void finishPattern();

	void setPlayState(EPlayState playState);

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

	void setLayerID(uint8_t layerID) { this->layerID = layerID; }
	uint8_t getLayerID() { return layerID; }

	bool isRunningPatternFromSequence() { return hasPatternSequence && patternSequence.getSequence().size(); }
	int getPatternIndex() { return patternIndex; }
	uint32_t getElapsedTime() { return (patternStartedAt == 0) ? 0 : millis() - patternStartedAt; }
	uint32_t getTransitionTimeElapsed() { return millis() - transitionStartedAt; }

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

	void setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex = -1, bool restartPattern = true, bool fadeOut = true);
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

	void enqueuePattern(PatternCode patternCode, bool waitToFinish = false);
	bool startPattern(PatternCode patternCode);

	void enqueuePatternAtIndex(int index, bool waitToFinish = false);
	bool startPatternAtIndex(int index);

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
