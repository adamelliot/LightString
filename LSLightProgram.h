#ifndef _LSLIGHTPROGRAM_H_
#define _LSLIGHTPROGRAM_H_

#include "LSPixelBuffer.h"
#include "LSZXSound.h"

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
	
	void setColorPalette(LSColorPalette *colorPalette);
	LSColorPalette *getColorPalette();

	void setZXSound(LSZXSound *zxSound);
	
	virtual uint8_t getPixelFormats();
	
	virtual uint8_t getMode();
	virtual uint8_t getModeCount();
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

	virtual void update();
};

#endif
