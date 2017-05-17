#pragma once

#include "types.h"
#include "PixelBuffer.h"

namespace LightString {

class ILightPattern {
protected:
	ILightLayer *layer = nullptr;

	// PatternID is set when a pattern is pulled out of the PatternProvider
	// It's use it for patterns to be able to identify one another. All 
	// Pattern ID setup is done from the PatternProvider
	pattern_id_t patternID = 0;
	uint8_t mode;
	EBlendMode blendMode;

public:
	static const uint8_t modeCount = 1;

	ILightPattern() : blendMode(BLEND_COPY) {}
	virtual ~ILightPattern() {}

	void setLayer(ILightLayer *layer) { this->layer = layer; }
	ILightLayer* getLayer() { return layer; }

	/**
	 * Only overload one of the setupMode functions. If you are using sequences and
	 * are expecting config objects use the 2 parameter call. The 2 parameter call
	 * takes precedence over the 1 parameter call.
	 */
	virtual void setupMode(uint8_t mode, PatternConfig *config) { setupMode(mode); }
	virtual void setupMode(uint8_t mode) {}
	virtual void patternFinished() {}
	
	void setMode(uint8_t mode, PatternConfig *config = nullptr) { this->mode = mode; setupMode(mode, config); }
	uint8_t getMode() { return mode; }
	virtual uint8_t getModeCount() { return modeCount; }

	void setBlendMode(EBlendMode blendMode) { this->blendMode = blendMode; }
	EBlendMode getBlendMode() { return blendMode; }

	ILightLayer *layerAbove();
	ILightLayer *layerBelow();

	bool startPatternAbove(PatternCode patternCode);
	bool startPatternAbove(pattern_id_t patternID, uint8_t mode = 0);
	
	bool startPatternBelow(PatternCode patternCode);
	bool startPatternBelow(pattern_id_t patternID, uint8_t mode = 0);

	bool startPatternOnLayer(uint8_t layerID, PatternCode patternCode);
	bool startPatternOnLayer(uint8_t layerID, pattern_id_t patternID, uint8_t mode = 0);

	virtual void setPalette(IPalette *palette) {}

	virtual size_t getSize() { return sizeof(*this); }

	virtual void setPixelBuffer(IPixelBuffer *pixelBuffer) {}
	virtual IPixelBuffer *getPixelBuffer() { return NULL; }

	virtual void setPixelBufferAdapter(IPixelBufferAdapter *) {}
	virtual IPixelBufferAdapter *getPixelBufferAdapter() { return nullptr; }

	virtual bool isFilterPattern() { return false; }

	pattern_id_t getPatternID() { return patternID; }
	void setPatternID(pattern_id_t val) { patternID = val; }

	virtual PatternCode getNextPatternCode() { return PatternCode(0xff, 0xff); /* Any pattern */ }

	// DEPRECATED
	virtual EPatternTransition getTransition() { return getOutTransition(); }

	virtual EPatternTransition getInTransition() { return TRANSITION_DEFAULT; }
	virtual EPatternTransition getOutTransition() { return TRANSITION_DEFAULT; }

	// Forces the length of the pattern transition. -1 Means default to container
	virtual int32_t getTransitionDuration() { return -1; }
	virtual int32_t getInTransitionDuration() { return getTransitionDuration(); }
	virtual int32_t getOutTransitionDuration() { return getTransitionDuration(); }

	// Flags that the next pattern should be loaded. This will take precedence over
	// any settings from sequences or layers.
	virtual bool isPatternFinished() { return false; }

	// Forces the length of the pattern. -1 Means default to container
	virtual int32_t getPatternDuration() { return -1; }

	// Called once per frame
	virtual void update(uint32_t ms) {}
};

class PatternProvider {
public:
	// Called to create a pattern that will be run in the system
	virtual ILightPattern *patternForID(pattern_id_t patternID, ILightLayer *layer = nullptr) {
		return nullptr;
	}

	// Pattern provider will need to clean up the pattern it created in `createPatternForID`
	virtual bool finishedWithPattern(ILightPattern *pattern, ILightLayer *layer = nullptr) {
		if (pattern) delete pattern;
		return true;
	}
};

template <template <typename> class PIXEL, typename FORMAT = uint8_t, unsigned MODE_COUNT = 1>
class TLightPattern : public ILightPattern {
protected:
	TPixelBuffer<PIXEL, FORMAT> *pixelBuffer;

public:
	TLightPattern() __attribute__((always_inline))
		: pixelBuffer(0) {}

	static const uint8_t modeCount = MODE_COUNT;
	virtual uint8_t getModeCount() { return MODE_COUNT; }

	void setPixelBuffer(IPixelBuffer *pixelBuffer) {
		this->pixelBuffer = (TPixelBuffer<PIXEL, FORMAT> *)pixelBuffer;
	}

	IPixelBuffer *getPixelBuffer() { return pixelBuffer; }

	size_t getSize() { return sizeof(*this); }
};

// Outputs a solid color for the pixel buffer
template <template <typename> class PIXEL, typename FORMAT = uint8_t>
class TSolidColorPattern : public TLightPattern<PIXEL, FORMAT> {
private:
	PIXEL<FORMAT> color;

public:
	TSolidColorPattern(PIXEL<FORMAT> color) : color(color) {}

	void update(uint32_t) { this->pixelBuffer->fillColor(color); }
};

// Filter patterns don't get their own private buffer, but operate on the
// output buffer.
class FilterLightPattern : public TLightPattern<TRGB, uint8_t> {
public:
	inline FilterLightPattern() {}
	bool isFilterPattern() { return true; } 
};

// Old style light patterns are designed to write on RGBu buffers
typedef FilterLightPattern LightPattern;
// typedef FilterLightPattern LightProgram;

};
