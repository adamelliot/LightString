#ifndef _LSLIGHTPROGRAM_H_
#define _LSLIGHTPROGRAM_H_

#include "LSPixelBuffer.h"

class LSLightProgram {
protected:
	uint8_t mode;
	uint8_t modeCount;
	LSPixelBuffer *pixelBuffer;

public:
	LSLightProgram(LSPixelBuffer *pixelBuffer);

	LSPixelBuffer *getPixelBuffer();
	
	virtual uint8_t getPixelFormats();

	uint8_t getMode();
	uint8_t getModeCount();
	virtual void setupMode(uint8_t mode);

	// 0 indicates no program ID
	virtual uint8_t getProgramID();
	virtual uint16_t getNextProgramCode();

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

#endif
