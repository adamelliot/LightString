#pragma once

#ifndef _LIGHTPROGRAM_H_
#define _LIGHTPROGRAM_H_

#include "buffers.h"

namespace LightString {
	
typedef enum {
	TRANSITION_OVERWRITE = 0, // Starts playing new program over existing data
	TRANSITION_WIPE, // Clears data and starts new program immediately
	TRANSITION_FADE_DOWN, // Forces all lights to fade while playing
	TRANSITION_FADE_UP, // Fades in all lights while playing
	TRANSITION_FREEZE_FADE, // Forces just the section to freeze, then fade down
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
	MODE_11 = 1 << 11,

	ANY_MODE = 0xff

} EProgramMode;

struct ProgramCode {
	uint8_t programID; // Name of program
	uint8_t copyID; // Which copy of the program
	uint8_t mode; // Which mode is specified

	inline ProgramCode(uint8_t programID = 0, uint8_t copyID = 0, uint8_t mode = 0) __attribute__((always_inline))
		: programID(programID), copyID(copyID), mode(mode) {}

	inline bool operator== (const ProgramCode &rhs) __attribute__((always_inline)) {
		return this->programID == rhs.programID && this->copyID == rhs.copyID && this->mode == rhs.mode;
	}
};

class IPalette;

class ILightLayer;
class ILightProgram;

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
	
	virtual ILightProgram *getActiveProgram() = 0;

	virtual bool startProgram(ProgramCode programCode) = 0;
	virtual bool startRandomProgram() = 0;
	virtual bool nextProgram() = 0;
	virtual bool prevProgram() = 0;
	
	virtual void stop() = 0;
	virtual void pause() = 0;
	virtual void unpause() = 0;
};

class ILightProgram {
protected:
	ILightLayer *layer;
	
	uint8_t mode;
	uint8_t modeCount;

	EBlendMode blendMode;

public:
	ILightProgram(uint8_t modeCount = 1) : modeCount(modeCount), blendMode(BLEND_COPY) {}
	
	void setLayer(ILightLayer *layer) { this->layer = layer; }
	ILightLayer* getLayer() { return layer; }

	virtual void setupMode(uint8_t mode) {}
	virtual void programFinished() {}
	
	void setMode(uint8_t mode) { this->mode = mode; setupMode(mode); }
	uint8_t getMode() { return mode; }
	uint8_t getModeCount() { return modeCount; }

	void setBlendMode(EBlendMode blendMode) { this->blendMode = blendMode; }
	EBlendMode getBlendMode() { return blendMode; }

	ILightLayer *layerAbove();
	ILightLayer *layerBelow();

	bool startProgramAbove(ProgramCode programCode);
	bool startProgramAbove(uint8_t programID, uint8_t copyID = 0, uint8_t mode = 0);
	
	bool startProgramBelow(ProgramCode programCode);
	bool startProgramBelow(uint8_t programID, uint8_t copyID = 0, uint8_t mode = 0);

	bool startProgramOnLayer(uint8_t layerID, ProgramCode programCode);
	bool startProgramOnLayer(uint8_t layerID, uint8_t programID, uint8_t copyID = 0, uint8_t mode = 0);

	virtual void setPalette(IPalette *palette) {}

	virtual size_t getSize() { return sizeof(*this); }

	virtual void setPixelBuffer(IPixelBuffer *pixelBuffer) {}
	virtual IPixelBuffer *getPixelBuffer() { return NULL; }

	virtual bool isFilterProgram() { return false; }

	// Program IDs are used to target effects from one program to another
	// if that isn't needed this can be left as zero
	virtual uint8_t getProgramID() { return 0; }

	virtual ProgramCode getNextProgramCode() { return ProgramCode(0xff, 0xff, 0xff); /* Any program */ }

	virtual EProgramTransition getTransition() { return getEndTransition(); }
	virtual EProgramTransition getBeginTransition() { return TRANSITION_FADE_UP; }
	virtual EProgramTransition getEndTransition() { return TRANSITION_FADE_DOWN; }

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

template <template <typename> class PIXEL, typename FORMAT = uint8_t>
class TLightProgram : public ILightProgram {
protected:
	
	// Set to which layer the program is being drawn on
	uint8_t layer;

	TPixelBuffer<PIXEL, FORMAT> *pixelBuffer;

public:
	TLightProgram(uint8_t modeCount = 1) __attribute__((always_inline))
		: ILightProgram(modeCount), layer(0), pixelBuffer(0) {}

	void setPixelBuffer(IPixelBuffer *pixelBuffer) {
		this->pixelBuffer = (TPixelBuffer<PIXEL, FORMAT> *)pixelBuffer;
	}

	IPixelBuffer *getPixelBuffer() { return pixelBuffer; }

	size_t getSize() { return sizeof(*this); }
};

// Filter programs don't get their own private buffer, but operate on the
// output buffer.
class FilterLightProgram : public TLightProgram<TRGB, uint8_t> {
public:
	inline FilterLightProgram(uint8_t modeCount = 1) __attribute__((always_inline))
		: TLightProgram(modeCount) {}

	bool isFilterProgram() { return true; } 
};

// Old style light programs are designed to write on RGBu buffers
typedef FilterLightProgram LightProgram;

};

#endif
