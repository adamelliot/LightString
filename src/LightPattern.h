#pragma once

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
	virtual ~ILightPattern() {}

	// If the clone method isn't supplied the system will default to using the
	// existing instance of the class and will report a warning
	virtual ILightPattern *clone() const { return nullptr; }
	
	void setLayer(ILightLayer *layer) { this->layer = layer; }
	ILightLayer* getLayer() { return layer; }

	// Set config is called before setupMode
	virtual void setConfig(PatternConfig *config) {}
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

	// DEPRECATED
	virtual EPatternTransition getTransition() { return getOutTransition(); }

	virtual EPatternTransition getInTransition() { return TRANSITION_DEFAULT; }
	virtual EPatternTransition getOutTransition() { return TRANSITION_DEFAULT; }

	// Forces the length of the pattern transition. -1 Means default to container
	virtual int32_t getTransitionDuration() { return -1; }

	// If false the pattern can only be activated by getNextPatternCode()
	virtual bool hideFromPatternList() { return false; }

	// Flags that the next pattern should be loaded. This will take precedence over
	// any settings from sequences or layers.
	virtual bool isPatternFinished() { return false; }

	// Forces the length of the pattern. -1 Means default to container
	virtual int32_t getPatternDuration() { return -1; }

	// Called once per frame
	virtual void update(uint32_t ms) {}
};

template <typename DERIVED, typename BASE>
class TCloneable : public BASE {
public:
	using BASE::BASE;

	virtual BASE *clone() const {
		return new DERIVED(static_cast<DERIVED const &>(*this));
	}
};

template <template <typename> class PIXEL, typename FORMAT = uint8_t>
class TLightPattern : public ILightPattern {
protected:
	TPixelBuffer<PIXEL, FORMAT> *pixelBuffer;

public:
	TLightPattern(uint8_t modeCount = 1) __attribute__((always_inline))
		: ILightPattern(modeCount), pixelBuffer(0) {}

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
