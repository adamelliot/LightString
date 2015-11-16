#ifndef _LIGHTPROGRAMMANAGER_H_
#define _LIGHTPROGRAMMANAGER_H_

#include "FastLED.h"
#include "LightProgram.h"

#define ALL_SECTIONS 0xffff

#define MAX_MODES 6
#define MAX_LIGHT_PROGRAMS 6
#define MAX_LIGHT_SECTIONS 1

#define VERBOSE

using namespace LightString;

namespace LightString {
	
const uint8_t kTransitionFrames = 30;
const uint8_t kMaxLayers = 2;

typedef enum {
	NONE = 0,
	STARTING,
	RUNNING,
	FINISHED
} ETransisionState;

typedef enum {
	PROGRAM_STARTED = 0,
	PROGRAM_FINISHED
} EProgramEvent;

typedef void (* ProgramEvent)(LightProgram &lightProgram, EProgramEvent event);

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

// TODO: Implement program grouping
/*
struct LightProgramGroup {
	uint8_t programID;
	// Pairs of codes and strip addreses
	uint32_t *programCodes;
};
*/

struct LightSection {
	uint8_t supportedPrograms[MAX_LIGHT_PROGRAMS];
	uint8_t programCount;

	int16_t programIndexOffset;

	PixelBuffer pixelBuffer;
	LightProgram *activeProgram;

	uint32_t programStartedAt;

	ETransisionState transitionState;

	inline LightSection() : programCount(0), programIndexOffset(0), 
		activeProgram(0), programStartedAt(0), transitionState(NONE) {}

	inline LightSection(CRGB* pixels, uint16_t length)
		: programCount(0), programIndexOffset(0), pixelBuffer(pixels, length), 
		activeProgram(0), programStartedAt(0), transitionState(NONE) {}
};

class ProgramManager {
private:
	LightSection lightSections[MAX_LIGHT_SECTIONS];
	uint8_t sectionCount;

	LightProgram *lightPrograms[MAX_LIGHT_PROGRAMS];
	ProgramCode programList[MAX_LIGHT_PROGRAMS * MAX_MODES];
	uint8_t programListLength;
	uint8_t programCount;
	uint8_t programOrder[MAX_LIGHT_PROGRAMS * MAX_MODES]; // Order of the program list
	uint8_t programIndex; // Index in the program order

	// Program Manager Timing
	int32_t maxProgramLength; // 0 or less means don't change based on time
	uint32_t lastTime;
	uint16_t msPerFrame;

	bool paused;
	uint32_t pauseStartedAt;

	uint8_t brightness;
	int16_t targetBrightness, adjustedBrightness;
	int16_t brightnessStep;
	
	// Events
	
	ProgramEvent programEventHandler;

	LightProgram *getProgram(ProgramCode programCode);
	LightProgram *getProgramForSection(ProgramCode programCode, LightSection &section);

	void selectProgramForSection(LightSection &section, ProgramCode programCode);
	// void selectProgramGroup(uint8_t programID);
	void nextProgramForSection(LightSection &section);
	void normalizeProgramIndices();

public:

	ProgramManager();

	void setMaxProgramLength(uint32_t maxProgramLength);
	int32_t getMaxProgramLength();

	void setProgramEventHandler(ProgramEvent programEventHandler) { this->programEventHandler = programEventHandler; }
	ProgramEvent getProgramEventHandler() { return programEventHandler; }

	void setMaxFPS(uint16_t targetFPS);
	void pause(bool blackout = true, bool fade = true);
	void unpause();
	void togglePause();

	void nudge(int32_t);

	void selectProgramCode(ProgramCode programCode);

	void selectProgram(uint8_t programID, uint8_t copyID = 0);
	void selectRandomProgram();
	void nextProgram();
	void prevProgram();
	void randomizeProgramOrder();

	void addLightProgram(LightProgram &program, uint16_t sections, uint8_t modes[], uint8_t modeCount);
	void addLightProgram(LightProgram &program, uint8_t modes[], uint8_t modeCount);
	void addLightProgram(LightProgram &program, EProgramMode programModes);
	void addLightProgram(LightProgram &program, uint16_t sections);
	void addLightProgram(LightProgram &program);

	// void addLightPrograms(LightProgram programs[], uint8_t count);

	LightSection *getLightSection(uint8_t);
	uint16_t addLightSection(CRGB *pixels, uint16_t length);

	void fadeDown();
	void fadeUp(bool forceZero = true);
	void setBrightness(uint8_t brightness);
	bool isTransitioning();
	void transitionBrightness();

	void finishProgramForSection(LightSection &section, uint32_t timeDelta);
	void loop();
};

};

#endif