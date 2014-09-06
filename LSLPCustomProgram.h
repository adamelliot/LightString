#ifndef _LSLPCUSTOMPROGRAM_H_
#define _LSLPCUSTOMPROGRAM_H_

#include "LSLightString.h"

static uint32_t customProgramBaseID = __COUNTER__;
// Generic Programs are 64 - 127
#define NEW_PROGRAM_ID() ((uint8_t)((uint32_t)__COUNTER__ - customProgramBaseID) + 0x40)

typedef void (*pcustom_setup_func)(uint8_t mode);
typedef void (*pcustom_update_func)(uint32_t ms);

class LSLPCustomProgram : public LSLightProgram {
protected:
	uint8_t programID;
	pcustom_setup_func setupFunc;
	pcustom_update_func updateFunc;

public:
	LSLPCustomProgram(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
		: LSLightProgram(pixelBuffer, colorPalette, colorFunc) {
		modeCount = 1;
	}

	void setSetupFunc(pcustom_setup_func setupFunc);
	void setUpdateFunc(pcustom_update_func updateFunc);

	uint8_t getProgramID() { return programID; }
	void setProgramID(uint8_t programID);

	void setupMode(uint8_t mode);
	void update(uint32_t ms);
};

LSLightProgram *factoryGradientCycle(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
