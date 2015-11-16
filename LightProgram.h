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

template <typename T = PixelBuffer>
class TLightProgram {
protected:
	uint8_t mode;
	uint8_t modeCount;
	
	// Set to which layer the program is being drawn on
	uint8_t layer;

	// Used by program manager to indicate if the program is running
	bool active;

	T *pixelBuffer;

public:
	TLightProgram(uint8_t modeCount = 1) __attribute__((always_inline))
		: modeCount(modeCount), layer(0), pixelBuffer(0) {}

	void setPixelBuffer(T *pixelBuffer) { this->pixelBuffer = pixelBuffer; }
	PixelBuffer *getPixelBuffer() { return pixelBuffer; }

	uint8_t getMode() { return mode; }
	uint8_t getModeCount() { return modeCount; }
	
	inline void setActive(bool a) __attribute__((always_inline)) { active = a; }
	inline bool isActive() __attribute__((always_inline)) { return active; }

	virtual void setupMode(uint8_t mode) { this->mode = mode; }

	// 0 indicates no program ID
	// Top 1 bit is reserverd
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

class LightProgram : public TLightProgram<PixelBuffer> {
public:
	inline LightProgram(uint8_t modeCount = 1) __attribute__((always_inline))
		: TLightProgram(modeCount) {}
};


};

#endif
