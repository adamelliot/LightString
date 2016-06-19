#pragma once

#include <cstdlib>
#include <stdint.h>

#define LIGHT_LAYER_TEMPLATE template <size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES>
#define LIGHT_LAYER_CLASS LightLayer<MAX_LIGHT_PROGRAMS, MAX_MODES>

#define LIGHT_SECTION_TEMPLATE template <template <typename> class PIXEL, typename FORMAT, size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES, size_t MAX_LAYERS>
#define LIGHT_SECTION_CLASS LightSection<PIXEL, FORMAT, MAX_LIGHT_PROGRAMS, MAX_MODES, MAX_LAYERS>

#define PROGRAM_MANAGER_TEMPLATE template <template <typename> class PIXEL, typename FORMAT, size_t MAX_LAYERS, size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES, size_t MAX_LIGHT_SECTIONS>
#define PROGRAM_MANAGER_CLASS PatternManager<PIXEL, FORMAT, MAX_LAYERS, MAX_LIGHT_PROGRAMS, MAX_MODES, MAX_LIGHT_SECTIONS>

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

const uint32_t kDefaultTransitionLength = 1000;
const uint8_t kTransitionFrames = 30;

typedef enum {
	TRANSITION_DONE = 0,
	TRANSITION_STARTING,
	TRANSITION_RUNNING
} ETransitionState;

typedef enum {
	PROGRAM_STARTED = 0,
	PROGRAM_PLAYING,
	PROGRAM_FINISHED, // Happens when a pattern ends
	PROGRAM_PAUSED,
	PROGRAM_STOPPED // Happens after a pattern finishes, but another isn't started
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

	ANY_MODE = 0xff

} EPatternMode;

struct PatternCode {
	uint8_t patternID; // Name of pattern
	uint8_t copyID; // Which copy of the pattern
	uint8_t mode; // Which mode is specified

	inline PatternCode(uint8_t patternID = 0, uint8_t copyID = 0, uint8_t mode = 0) __attribute__((always_inline))
		: patternID(patternID), copyID(copyID), mode(mode) {}

	inline bool operator== (const PatternCode &rhs) __attribute__((always_inline)) {
		return this->patternID == rhs.patternID && this->copyID == rhs.copyID && this->mode == rhs.mode;
	}
};

struct IPalette;

class ILightLayer;
class ILightPattern;

typedef void (* PatternEvent)(ILightPattern &lightPattern, EPlayState event);

class ILightSection {
public:

	virtual IPixelBuffer *getOutputBuffer() = 0;

	virtual uint8_t getMaxLayers() = 0;
	virtual ILightLayer *getLayer(uint8_t layerID) = 0;

	virtual IPixelBuffer *lockBuffer() = 0;
	virtual void unlockBuffer(IPixelBuffer *buffer) = 0;
	virtual bool addBuffer(IPixelBuffer *buffer) = 0;
};

class ILightLayer {
public:
	
	virtual void setLayerID(uint8_t layerID) = 0;
	virtual uint8_t getLayerID() = 0;
	
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