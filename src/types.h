#pragma once

#include <vector>
#include <cstdlib>
#include <stdint.h>

#define LIGHT_LAYER_TEMPLATE template <typename FORMAT>//, size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES>
#define LIGHT_LAYER_CLASS LightLayer<FORMAT>//, MAX_LIGHT_PROGRAMS, MAX_MODES>

#define LIGHT_SECTION_TEMPLATE template <template <typename> class PIXEL, typename FORMAT, template <typename> class OUTPUT_PIXEL>//size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES, size_t MAX_LAYERS>
#define LIGHT_SECTION_CLASS LightSection<PIXEL, FORMAT, OUTPUT_PIXEL>//MAX_LIGHT_PROGRAMS, MAX_MODES, MAX_LAYERS>

#define PATTERN_MANAGER_TEMPLATE template <template <typename> class PIXEL, typename FORMAT, template <typename> class OUTPUT_PIXEL>
#define PATTERN_MANAGER_CLASS PatternManager<PIXEL, FORMAT, OUTPUT_PIXEL>//MAX_LAYERS, MAX_LIGHT_PROGRAMS, MAX_MODES, MAX_LIGHT_SECTIONS>

namespace LightString {

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
const uint8_t kTransitionFrames = 30;

typedef enum {
	TRANSITION_DONE = 0,
	TRANSITION_STARTING,
	TRANSITION_RUNNING
} ETransitionState;

typedef enum {
	PATTERN_STARTED = 0,
	PATTERN_PLAYING,
	PATTERN_FINISHED, // Happens when a pattern ends
	PATTERN_PAUSED,
	PATTERN_STOPPED // Happens after a pattern finishes, but another isn't started
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


struct PatternCode {
	uint8_t patternID = 0; // Name of pattern
	uint8_t copyID = 0; // Which copy of the pattern
	uint8_t mode = 0; // Which mode is specified

	inline PatternCode(uint8_t patternID = 0, uint8_t copyID = 0, uint8_t mode = 0) :
		patternID(patternID), copyID(copyID), mode(mode) {}

	inline PatternCode(const PatternCode& code) :
		patternID(code.patternID), copyID(code.copyID), mode(code.mode) {}

	inline bool operator== (const PatternCode &rhs) __attribute__((always_inline)) {
		return this->patternID == rhs.patternID && this->copyID == rhs.copyID && this->mode == rhs.mode;
	}
};

struct IPalette;

class ILightLayer;
class ILightPattern;

typedef void (* PatternEvent)(ILightPattern &lightPattern, EPlayState event, void *userData);

class ILightSection {
public:

	virtual IPixelBuffer *getOutputBuffer() = 0;

	virtual uint8_t getTotalLayers() = 0;
	virtual ILightLayer *getLayer(uint8_t layerID) = 0;
	virtual void ensureLayerExists(uint8_t layerID) = 0;

	virtual IPixelBuffer *lockBuffer() = 0;
	virtual void unlockBuffer(IPixelBuffer *buffer) = 0;
	virtual bool addBuffer(IPixelBuffer *buffer) = 0;
};

struct PatternConfig {
	// Pattern Manager Timing
	// -1 - Default to container
	//  0 - Run for ever
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
	PatternCode code;

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
};

class PatternSequence {
private:
	std::vector<PatternCue> sequence;

public:

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
	PatternCue &getPatternCue(uint16_t index) { return sequence[index]; }
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
	
	virtual void setLayerID(uint8_t layerID) = 0;
	virtual uint8_t getLayerID() = 0;

	virtual void setConfig(const LightLayerConfig &config) = 0;
	virtual LightLayerConfig &getConfig() = 0;

	virtual void setPalette(IPalette *) {}

	virtual void setLightSection(ILightSection *section) = 0;
	virtual ILightSection *getLightSection() = 0;
	
	virtual ILightPattern *getActivePattern() = 0;

	virtual bool startPattern(PatternCode patternCode) = 0;
	virtual bool startRandomPattern() = 0;
	virtual bool nextPattern() = 0;
	virtual bool prevPattern() = 0;
	
	virtual void stop() = 0;
	virtual void pause() = 0;
	virtual void unpause() = 0;
};

};