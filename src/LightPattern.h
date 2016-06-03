#pragma once

#ifndef _LIGHTPROGRAM_H_
#define _LIGHTPROGRAM_H_

#include "buffers.h"

namespace LightString {
	
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

class IPalette;

class ILightLayer;
class ILightPattern;

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

class ILightPattern {
protected:
	ILightLayer *layer;
	
	uint8_t mode;
	uint8_t modeCount;

	EBlendMode blendMode;

public:
	ILightPattern(uint8_t modeCount = 1) : modeCount(modeCount), blendMode(BLEND_COPY) {}
	
	void setLayer(ILightLayer *layer) { this->layer = layer; }
	ILightLayer* getLayer() { return layer; }

	virtual void setupMode(uint8_t mode) {}
	virtual void patternFinished() {}
	
	void setMode(uint8_t mode) { this->mode = mode; setupMode(mode); }
	uint8_t getMode() { return mode; }
	uint8_t getModeCount() { return modeCount; }

	void setBlendMode(EBlendMode blendMode) { this->blendMode = blendMode; }
	EBlendMode getBlendMode() { return blendMode; }

	ILightLayer *layerAbove();
	ILightLayer *layerBelow();

	bool startPatternAbove(PatternCode patternCode);
	bool startPatternAbove(uint8_t patternID, uint8_t copyID = 0, uint8_t mode = 0);
	
	bool startPatternBelow(PatternCode patternCode);
	bool startPatternBelow(uint8_t patternID, uint8_t copyID = 0, uint8_t mode = 0);

	bool startPatternOnLayer(uint8_t layerID, PatternCode patternCode);
	bool startPatternOnLayer(uint8_t layerID, uint8_t patternID, uint8_t copyID = 0, uint8_t mode = 0);

	virtual void setPalette(IPalette *palette) {}

	virtual size_t getSize() { return sizeof(*this); }

	virtual void setPixelBuffer(IPixelBuffer *pixelBuffer) {}
	virtual IPixelBuffer *getPixelBuffer() { return NULL; }

	virtual bool isFilterPattern() { return false; }

	// Pattern IDs are used to target effects from one pattern to another
	// if that isn't needed this can be left as zero
	virtual uint8_t getPatternID() { return 0; }

	virtual PatternCode getNextPatternCode() { return PatternCode(0xff, 0xff, 0xff); /* Any pattern */ }

	virtual EPatternTransition getTransition() { return getEndTransition(); }
	virtual EPatternTransition getBeginTransition() { return TRANSITION_FADE_UP; }
	virtual EPatternTransition getEndTransition() { return TRANSITION_FADE_DOWN; }

	// If false the pattern can only be activated by getNextPatternCode()
	virtual bool hideFromPatternList() { return false; }

	// Flags that the next pattern should be loaded
	virtual bool isPatternFinished() { return false; }

	// Returns how long this pattern should run in ms
	virtual int32_t getPatternLength() { return -1; /* -1 means forever */ }

	// Used to pass data into a running pattern via the pattern manager
	// TODO: Is there a better way to handle this?
	virtual void nudge(int32_t data) {}

	virtual void update(uint32_t ms) {}
};

template <template <typename> class PIXEL, typename FORMAT = uint8_t>
class TLightPattern : public ILightPattern {
protected:
	
	// Set to which layer the pattern is being drawn on
	uint8_t layer;

	TPixelBuffer<PIXEL, FORMAT> *pixelBuffer;

public:
	TLightPattern(uint8_t modeCount = 1) __attribute__((always_inline))
		: ILightPattern(modeCount), layer(0), pixelBuffer(0) {}

	void setPixelBuffer(IPixelBuffer *pixelBuffer) {
		this->pixelBuffer = (TPixelBuffer<PIXEL, FORMAT> *)pixelBuffer;
	}

	IPixelBuffer *getPixelBuffer() { return pixelBuffer; }

	size_t getSize() { return sizeof(*this); }
};

// Filter patterns don't get their own private buffer, but operate on the
// output buffer.
class FilterLightPattern : public TLightPattern<TRGB, uint8_t> {
public:
	inline FilterLightPattern(uint8_t modeCount = 1) __attribute__((always_inline))
		: TLightPattern(modeCount) {}

	bool isFilterPattern() { return true; } 
};

// Old style light patterns are designed to write on RGBu buffers
typedef FilterLightPattern LightPattern;

};

#endif
