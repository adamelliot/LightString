#pragma once

#include <vector>
#include <cstdlib>
#include <stdint.h>

#include <exception>

#define LIGHT_LAYER_TEMPLATE template <typename FORMAT>//, size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES>
#define LIGHT_LAYER_CLASS LightLayer<FORMAT>//, MAX_LIGHT_PROGRAMS, MAX_MODES>

#define LIGHT_SECTION_TEMPLATE template <template <typename> class PIXEL, typename FORMAT, template <typename> class OUTPUT_PIXEL>//size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES, size_t MAX_LAYERS>
#define LIGHT_SECTION_CLASS LightSection<PIXEL, FORMAT, OUTPUT_PIXEL>//MAX_LIGHT_PROGRAMS, MAX_MODES, MAX_LAYERS>

#define PATTERN_MANAGER_TEMPLATE template <template <typename> class PIXEL, typename FORMAT, template <typename> class OUTPUT_PIXEL>
#define PATTERN_MANAGER_CLASS PatternManager<PIXEL, FORMAT, OUTPUT_PIXEL>//MAX_LAYERS, MAX_LIGHT_PROGRAMS, MAX_MODES, MAX_LIGHT_SECTIONS>

namespace LightString {

typedef enum {
	SECTION_DOES_NOT_EXIST  = 1,
	LAYER_DOES_NOT_EXIST    = 2
} EErrorCode;

class Exception : public std::exception {
	EErrorCode errorCode;

	virtual const char *what() const throw() {
		switch (errorCode) {
			case SECTION_DOES_NOT_EXIST:
				return "Section specified does not exist";
			case LAYER_DOES_NOT_EXIST:
				return "Layer specified does not exist";
		}
	}
public:
	Exception(EErrorCode code) : errorCode(code) {}

};

typedef enum {
	BLEND_COPY = 0,
	BLEND_ADD
	
} EBlendMode;

// Place holder type for all generated TPixelBuffers
struct IPixelBuffer {
	virtual uint16_t getLength() const = 0;
	virtual void clear() = 0;
};

const uint32_t kDefaultTransitionDuration = 1000;
const uint32_t kDefaultFadeOutDuration = 500;
const uint8_t kTransitionFrames = 30;

typedef enum {
	TRANSITION_DONE = 0,
	TRANSITION_STARTING,
	TRANSITION_RUNNING
} ETransitionState;

typedef enum {
	PATTERN_SETUP = 0,
	PATTERN_STARTED,
	PATTERN_PLAYING_IN_TRANSITION,
	PATTERN_PLAYING,
	PATTERN_PLAYING_OUT_TRANSITION,
	PATTERN_FINISHED, // Happens when a pattern ends
	PATTERN_PAUSED,
	PATTERN_STOPPED, // Happens after a pattern finishes, but another isn't started

	PATTERN_HOLD = 10,
	PATTERN_UNHOLD
} EPlayState;

typedef enum {
	PLAY_MODE_CONTINUOUS = 0, // Keep playing another pattern
	PLAY_MODE_ONCE, // Play the pattern then stop
	PLAY_MODE_REPEAT // Keep repeating the same pattern
} EPlayMode;

typedef enum {
	TRANSITION_OVERWRITE = 0, // Starts playing new pattern over existing data
	TRANSITION_WIPE, // Clears data and starts new pattern immediately
	TRANSITION_FADE_DOWN, // Forces all lights to fade while playing
	TRANSITION_FADE_UP, // Fades in all lights while playing
	TRANSITION_FREEZE_FADE, // Forces just the section to freeze, then fade down

	TRANSITION_DEFAULT = 0xff // Use the transition from the container
} EPatternTransition;

/*! Used to describe the behavior after a pattern finishes and which pattern to load.
	If any of the actions fail the `LAYER` is put in a STOPPED state.
*/
enum EPlayOutAction {
	LOAD_NEXT = 1, /*!< Next will load the next in the sequence.
		If the pattern specifies a specific next pattern through getNextPatternCode
		that will be loaded instead of the next in the sequence. */
	LOAD_PREVIOUS, /*!< Load the previous pattern in the sequence */

	LOAD_ENQUEUED_PATTERN, /*!< Load the patten code enqueued with enqueuePattern */
	LOAD_ENQUEUED_INDEX, /*!< Load the patten cue enqueued with enqueuePatternAtIndex */

	FADE_TO_STOP, /*!< Cause the layer to fade out then `STOP` */
	FREEZE_FADE_TO_STOP /*!< Cause the layer to fade out in a frozen state then `STOP` */
};

/*!	Collection of options to control how setting a new pattern sequence
	loads the new sequence and processes the running pattern.
*/
enum ESetPatternSequenceFlags {
	RETAIN_PATTERN			= 0x01, /*!< If the pattern at `newPlayIndex` is
		the same as the current playing pattern sending this flag will cause
		that pattern to keep playing. */
	FADE_OUT				= 0x02, /*!< If the sequence is loading and stopping
		a playing pattern this controls if it will fade out or stop immediately. */
	TRANSITION_PATTERN		= 0x04, /*!< If passing a valid `newPlayIndex` this
		will control if it should start immediately (default) or if it should
		transition. If playing from a different sequence setting this will
		cause the pattern to transition from the current the the one specified
		by `newPlayIndex`. */
	DONT_STOP_CURRENT		= 0x08, /*!< This will leave the current pattern
		playing, but set everything up so the next pattern loaded will be
		the first in the sequence or the one set by `newPlayIndex`. */
};

typedef enum {
	MODE_0  = 1 << 0,
	MODE_1  = 1 << 1,
	MODE_2  = 1 << 2,
	MODE_3  = 1 << 3,
	MODE_4  = 1 << 4,
	MODE_5  = 1 << 5,
	MODE_6  = 1 << 6,
	MODE_7  = 1 << 7,
	MODE_8  = 1 << 8,
	MODE_9  = 1 << 9,
	MODE_10 = 1 << 10,
	MODE_11 = 1 << 11,

	ALL_MODES = 0xff

} EPatternMode;

template <typename FORMAT>
struct TColorFormatHelper {
	static FORMAT getMaxValue();
	static FORMAT scale(FORMAT a, FORMAT b);
};

template <>
inline uint8_t TColorFormatHelper<uint8_t>::getMaxValue() { return 0xff; }

template <>
inline float TColorFormatHelper<float>::getMaxValue() { return 1.0f; }

template <>
inline uint8_t TColorFormatHelper<uint8_t>::scale(uint8_t a, uint8_t b) {
	return ((uint16_t)a * (uint16_t)(b + 1)) >> 8;
}

template <>
inline float TColorFormatHelper<float>::scale(float a, float b) {
	return a * b;
}

typedef uint8_t pattern_id_t;

struct PatternCode {
	pattern_id_t patternID = 0; // Name of pattern
	uint8_t mode = 0; // Which mode is specified

	PatternCode(pattern_id_t patternID = 0, uint8_t mode = 0) :
		patternID(patternID), mode(mode) {}

	PatternCode(const PatternCode& code) :
		patternID(code.patternID), mode(code.mode) {}

	bool operator== (const PatternCode &rhs) __attribute__((always_inline)) {
		return this->patternID == rhs.patternID && this->mode == rhs.mode;
	}

	bool operator!= (const PatternCode &rhs) __attribute__((always_inline)) {
		return this->patternID != rhs.patternID || this->mode != rhs.mode;
	}

	bool isAnyPattern() { return *this == anyPattern(); }
	bool isNoPattern() { return *this == noPattern(); }

	static PatternCode anyPattern() { return {0xff, 0xff}; }
	static PatternCode noPattern() { return {0xff, 0xfe}; }
};

struct IPalette;

class ILightLayer;
class ILightPattern;

class PatternProvider;

typedef void (* PatternEvent)(ILightPattern *lightPattern, EPlayState event, void *userData);

class ILightSection {
protected:
	uint8_t sectionID = 0;

public:
	virtual ~ILightSection() {}

	virtual IPixelBuffer *getOutputBuffer() = 0;

	virtual uint8_t getTotalLayers() = 0;
	virtual ILightLayer *getLayer(uint8_t layerID) = 0;
	virtual void ensureLayerExists(uint8_t layerID) = 0;

	virtual IPixelBuffer *lockBuffer() = 0;
	virtual void unlockBuffer(IPixelBuffer *buffer) = 0;
	virtual bool addBuffer(IPixelBuffer *buffer) = 0;

	virtual PatternProvider &getPatternProvider() = 0;

	virtual int32_t getFadeDuration() { return kDefaultFadeOutDuration; }
	virtual void setFadeDuration(int32_t val) {}

	void setSectionID(uint8_t val) { sectionID = val; }
	uint8_t getSectionID() { return sectionID; }
};

struct PatternConfig {
	// Pattern Manager Timing
	// -1 - Default to container
	//  0 - Run for forever
	// If the pattern specifies a time and this is set, the pattern will take precedent
	// patternDuration includes the amount of time spend in transitionDuration
	int32_t patternDuration = 0;

	// How long (in ms) the transition will last before running pattern
	int32_t inTransitionDuration = kDefaultTransitionDuration;
	int32_t outTransitionDuration = kDefaultTransitionDuration;

	// Which transition to run during the transitionDuration
	EPatternTransition inTransition = TRANSITION_FADE_UP;

	// Which transition to run during the outDuration
	EPatternTransition outTransition = TRANSITION_FADE_DOWN;

	// Any extra structure that the user may want to have when the pattern starts
	void *config = nullptr;
};

struct PatternCue : public PatternConfig {
public:
	// Which pattern and code to run
	PatternCode code = 0xff;

	PatternCue() {}
	PatternCue(PatternCode code, int32_t patternDuration, int32_t inTransitionDuration, int32_t outTransitionDuration,
		EPatternTransition inTransition, EPatternTransition outTransition, void *config = nullptr)
	{
		this->code = code;
		this->patternDuration = patternDuration;
		this->inTransitionDuration = inTransitionDuration;
		this->outTransitionDuration = outTransitionDuration;
		this->inTransition = inTransition;
		this->outTransition = outTransition;
		this->config = config;
	}

	PatternCue(PatternConfig &rhs) {
		this->patternDuration = rhs.patternDuration;
		this->inTransitionDuration = rhs.inTransitionDuration;
		this->outTransitionDuration = rhs.outTransitionDuration;
		this->inTransition = rhs.inTransition;
		this->outTransition = rhs.outTransition;
		this->config = rhs.config;
	}
};

class IPatternSequence {
public:
	virtual ~IPatternSequence() {}

	virtual void addPatternCode(const PatternCode &patternCode) = 0;

	virtual PatternCode getPatternCode(size_t index) const = 0;
	virtual PatternCue getPatternCue(size_t index) const = 0;

	virtual size_t size() const = 0;
	virtual void clear() = 0;

	virtual void shuffle() = 0;
};

/**
 * A sequence that just is a list of pattern codes, and uses the layer's
 * PatternConfig to determine how duration and transitions work.
 */
class SimplePatternSequence : public IPatternSequence {
private:
	PatternConfig &config;
	std::vector<PatternCode> sequence;

public:
	SimplePatternSequence(PatternConfig &config) : config(config) {}
	virtual ~SimplePatternSequence() {}

	void addPatternCode(const PatternCode &patternCode) {
		sequence.emplace_back(patternCode);
	}

	PatternCode getPatternCode(size_t index) const { return sequence[index]; }
	PatternCue getPatternCue(size_t index) const {
		PatternCue ret = config;
		ret.code = sequence[index];
		return ret;
	}

	size_t size() const { return sequence.size();	}
	void clear() { sequence.clear(); }

	void shuffle() {
		auto size = sequence.size();
		if (size <= 1) return;

		for (size_t i = 0; i < sequence.size(); i++) {
			size_t j = (i + random() / (0x7ffffff / (sequence.size() - i) + 1)) % sequence.size();
			auto t = sequence[j];
			sequence[j] = sequence[i];
			sequence[i] = t;
		}
	}
};

/**
 * A sequence of pattern cues that each can control duration and transition details.
 */
class PatternSequence : public IPatternSequence {
private:
	std::vector<PatternCue> sequence;

public:
	virtual ~PatternSequence() {}

	void addPatternCode(const PatternCode &patternCode) {
		addPatternCue(patternCode);
	}

	void addPatternCue(const PatternCue &patternCue) {
		sequence.emplace_back(patternCue);
	}

	void addPatternCue(PatternCode code, int32_t patternDuration = 0,
		EPatternTransition inTransition = TRANSITION_FADE_UP,
		EPatternTransition outTransition = TRANSITION_FADE_DOWN,
		int32_t transitionDuration = kDefaultTransitionDuration, void *config = nullptr) {
		sequence.emplace_back(PatternCue(code, patternDuration, transitionDuration, 
			transitionDuration, inTransition, outTransition, config));
	}

	std::vector<PatternCue> &getSequence() { return sequence; }
	const std::vector<PatternCue> &getSequence() const { return sequence; }

	PatternCode getPatternCode(size_t index) const { return sequence[index].code; }
	PatternCue getPatternCue(size_t index) const { return sequence[index]; }

	size_t size() const { return sequence.size();	}
	void clear() { sequence.clear(); }

	void shuffle() {
		auto size = sequence.size();
		if (size <= 1) return;

		for (size_t i = 0; i < sequence.size(); i++) {
			size_t j = (i + random() / (0x7ffffff / (sequence.size() - i) + 1)) % sequence.size();
			auto t = sequence[j];
			sequence[j] = sequence[i];
			sequence[i] = t;
		}
	}
};

struct LightLayerConfig : PatternConfig {
	// Method called when a pattern event happens
	PatternEvent patternEventHandler = nullptr;

	// Data to be handed back to the patterEventHandler
	void *patternEventUserData = nullptr;

	// What playback mode the layer is in
	EPlayMode playMode = PLAY_MODE_CONTINUOUS;
};

class ILightLayer {
public:
	virtual ~ILightLayer() {}

	virtual EPlayState getPlayState() = 0;

	virtual void setLayerID(uint8_t layerID) = 0;
	virtual uint8_t getLayerID() = 0;

	virtual bool isRunningPatternFromSequence() = 0;
	virtual int getPatternIndex() = 0;
	virtual uint32_t getElapsedTime() = 0;
	virtual uint32_t getTransitionTimeElapsed() = 0;

	virtual void setConfig(const LightLayerConfig &config) = 0;
	virtual LightLayerConfig &getConfig() = 0;

	virtual void setPalette(IPalette *) {}

	virtual void setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex = -1, uint32_t flags = 0) = 0;
	virtual void clearPatternSequence(bool fadeOut = true, bool stopIfPlayingFromSequence = true) = 0;

	virtual void setLightSection(ILightSection *section) = 0;
	virtual ILightSection *getLightSection() = 0;
	
	virtual ILightPattern *getActivePattern() = 0;

	virtual bool isPatternHeld() const = 0;

	virtual int getPatternIndex() const = 0;

	virtual void enqueuePattern(PatternCode patternCode, bool waitToFinish = false) = 0;
	virtual bool startPattern(PatternCode patternCode, bool transition = false) = 0;

	virtual bool enqueuePatternAtIndex(size_t index, bool waitToFinish = false) = 0;
	virtual bool startPatternAtIndex(size_t index, bool transition = false) = 0;

	virtual bool startRandomPattern(bool transition = false) = 0;
	virtual bool nextPattern(bool transition = false) = 0;
	virtual bool prevPattern(bool transition = false) = 0;

	virtual void shufflePatterns() = 0;

	virtual void stop(bool fadeOut = true) = 0;
	virtual void pause() = 0;
	virtual void unpause() = 0;

	virtual void hold() = 0;
	virtual void unhold() = 0;

};

};