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
} TProgramTransition;

class LightProgram {
protected:
	uint8_t mode;
	uint8_t modeCount;
	PixelBuffer *pixelBuffer;

public:
	LightProgram(PixelBuffer *pixelBuffer = 0);

	void setpixelBuffer(PixelBuffer *pixelBuffer);
	PixelBuffer *getPixelBuffer();

	uint8_t getMode();
	uint8_t getModeCount();
	virtual void setupMode(uint8_t mode);

	// 0 indicates no program ID
	virtual uint8_t getProgramID();
	virtual uint16_t getNextProgramCode();
	
	virtual TProgramTransition getTransition();

	// Indicates that the program wants to use the palette of the last program
	virtual bool usePreviousPalette();
	// If false the program can only be activated by getNextProgramCode()
	virtual bool hideFromProgramList();
	// Flags that the next program should be loaded
	virtual bool isProgramFinished();
	// Returns how long this program should run in ms
	virtual int32_t getProgramLength(); // -1 = forever

	virtual void setConfig(void *config);
	virtual void *getConfig();

	virtual void nudge(int32_t data);

	virtual void update(uint32_t ms);
};

};

#endif
