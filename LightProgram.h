#ifndef _LIGHTPROGRAM_H_
#define _LIGHTPROGRAM_H_

#include "drawing.h"

using namespace LightString;

namespace LightString {
	
typedef enum {
	OVERWRITE = 0, // Starts playing new program over existing data
	WIPE, // Clears data and starts new program immediately
	FADE_DOWN, // Forces all lights to fade while playing
	FREEZE_FADE // Forces just the section to freeze, then fade down
} EProgramTransition;

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
	MODE_11 = 1 << 11

} EProgramMode;

class ILightLayer;

class ILightProgram {
protected:
	ILightLayer *layer;
	
	uint8_t mode;
	uint8_t modeCount;

	EBlendMode blendMode;

public:
	ILightProgram(uint8_t modeCount = 1) : modeCount(modeCount) {}
	
	void setLayer(ILightLayer *layer) { this->layer = layer; }
	ILightLayer* getLayer() { return layer; }

	virtual void setupMode(uint8_t mode) {}
	void setMode(uint8_t mode) { this->mode = mode; setupMode(mode); }
	uint8_t getMode() { return mode; }
	uint8_t getModeCount() { return modeCount; }
	
	virtual void setPixelBuffer(IPixelBuffer *pixelBuffer) {}
	virtual IPixelBuffer *getPixelBuffer() { return NULL; }
	
	virtual bool isFilterProgram() { return false; }

	// Program IDs are used to target effects from one program to another
	// if that isn't needed this can be left as zero
	virtual uint8_t getProgramID() { return 0; }
	
	// TODO: Adjust this to work directly with ProgramCode
	virtual uint16_t getNextProgramCode() { return 0; /* Any program */ }

	virtual EProgramTransition getTransition() { return FADE_DOWN; }

	// If false the program can only be activated by getNextProgramCode()
	virtual bool hideFromProgramList() { return false; }

	// Flags that the next program should be loaded
	virtual bool isProgramFinished() { return false; }

	// Returns how long this program should run in ms
	virtual int32_t getProgramLength() { return -1; /* -1 means forever */ }

	// Used to pass data into a running program via the program manager
	// TODO: Is there a better way to handle this?
	virtual void nudge(int32_t data) {}

	virtual void update(uint32_t ms) {}
};

template <typename PIXEL = Pixel>
class TLightProgram : public ILightProgram {
protected:
	
	// Set to which layer the program is being drawn on
	uint8_t layer;

	TPixelBuffer<PIXEL> *pixelBuffer;

public:
	TLightProgram(uint8_t modeCount = 1) __attribute__((always_inline))
		: ILightProgram(modeCount), layer(0), pixelBuffer(0) {}

	void setPixelBuffer(IPixelBuffer *pixelBuffer) {
		this->pixelBuffer = (TPixelBuffer<PIXEL> *)pixelBuffer;
	}

	IPixelBuffer *getPixelBuffer() { return pixelBuffer; }
};

class LightProgram : public TLightProgram<CRGB> {
public:
	inline LightProgram(uint8_t modeCount = 1) __attribute__((always_inline))
		: TLightProgram(modeCount) {}
};

// Filter programs don't get their own private buffer, but operate on the
// output buffer.
class FilterLightProgram : public TLightProgram<CRGB> {
public:
	inline FilterLightProgram(uint8_t modeCount = 1) __attribute__((always_inline))
		: TLightProgram(modeCount) {}

	bool isFilterProgram() { return true; } 
};


};

#endif
