#pragma once

#ifndef _LIGHTPROGRAM_H_
#define _LIGHTPROGRAM_H_

#include "types.h"
#include "PixelBuffer.h"

namespace LightString {

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
// typedef FilterLightPattern LightProgram;

};

#endif
