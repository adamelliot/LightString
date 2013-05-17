#ifndef _LSLIGHTPROGRAM_H_
#define _LSLIGHTPROGRAM_H_

#include "LSPixelBuffer.h"
#include "LSZXSound.h"

enum LSProgramCodes {
	// Named programs have no mode code
	RANDOM_PROGRAM = 0xff01,
	PREV_PROGRAM   = 0xff02,
	NEXT_PROGRAM   = 0xff03,
	NO_PROGRAM     = 0xffff
};

typedef LSColorPalette *(*pcolor_palette_factory_func)();

class LSLightProgram {
protected:
	uint8_t mode;
	pcolor_func colorFunc;
	LSColorPalette *colorPalette;
	LSPixelBuffer *pixelBuffer;
	LSZXSound *zxSound;

public:
	LSLightProgram(LSPixelBuffer *pixelBuffer, LSColorPalette *colorPalette, pcolor_func colorFunc);

	LSPixelBuffer *getPixelBuffer();

	void setColorFunc(pcolor_func colorFunc);
	pcolor_func getColorFunc();
	
	void setColorPalette();
	LSColorPalette *getColorPalette();
	
	void setZXSound(LSZXSound *zxSound);
	
	virtual uint8_t getPixelFormats();
	
	uint8_t uint8_t getMode();
	virtual uint8_t getModeCount();
	virtual void setupMode(uint8_t mode);

	// 128 program modes, -1 indicates no program ID
	virtual uint8_t getProgramID();
	virtual uint16_t getNextProgramCode();

	// If false the program can only be activated by 
	virtual bool hideFromProgramQueue();
	// Flags that the next program should be loaded
	virtual bool isProgramFinished();
	// Returns how long this program should run in ms
	virtual int32_t getProgramLength(); // -1 = forever
	virtual uint8_t getFrameRate();

	virtual void setConfig(void *config);
	virtual void *getConfig();

	virtual void update();
};

#endif
