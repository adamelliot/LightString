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

typedef enum {
	NONE = 0,
	STARTING,
	RUNNING,
	FINISHED
} TTransisionState;

typedef enum {
	PROGRAM_STARTED = 0,
	PROGRAM_FINISHED
} TProgramEvent;

typedef void (* ProgramEvent)(LightProgram &lightProgram, TProgramEvent event);

// typedef struct light_section_s light_section_t, *plight_section_t;

// TODO: Implement program grouping
// NOTE: Group Program IDs are >= 0x80
struct LightProgramGroup {
	uint8_t programID;
	// Pairs of codes and strip addreses
	uint32_t *programCodes;
};

struct LightSection {
	uint8_t supportedPrograms[MAX_LIGHT_PROGRAMS];
	uint8_t programCount;

	int16_t programIndexOffset;

	PixelBuffer pixelBuffer;
	LightProgram *activeProgram;

	uint32_t programStartedAt;

	TTransisionState transitionState;

	inline LightSection() : programCount(0), programIndexOffset(0), 
		programStartedAt(0), activeProgram(0), transitionState(NONE) {}

	inline LightSection(CRGB* pixels, uint16_t length)
		: pixelBuffer(pixels, length), programCount(0), programIndexOffset(0), 
		programStartedAt(0), activeProgram(0), transitionState(NONE) {}
};

class ProgramManager {
private:
	LightSection lightSections[MAX_LIGHT_SECTIONS];
	uint8_t sectionCount;
	
	uint8_t sectionsChanged;

	LightProgram *lightPrograms[MAX_LIGHT_PROGRAMS];
	uint16_t programList[MAX_LIGHT_PROGRAMS * MAX_MODES];
	uint8_t programListLength;
	uint8_t programCount;
	uint8_t programOrder[MAX_LIGHT_PROGRAMS * MAX_MODES]; // Order of the program list
	uint8_t programIndex; // Index in the program order

	// Program Manager Timing
	int32_t maxProgramLength;
	uint32_t lastTime;
	uint16_t msPerFrame;

	bool paused;
	uint32_t pauseStartedAt;

	uint8_t brightness;
	int16_t targetBrightness, adjustedBrightness;
	int16_t brightnessStep;

	LightProgram *getProgram(uint8_t programID);
	LightProgram *getProgramForSection(uint8_t programID, LightSection &section);
	void selectProgramForSection(LightSection &section, uint8_t programID, uint8_t programMode, bool keepPalette = false);
	void selectProgramGroup(uint8_t programID);
	void nextProgramForSection(LightSection &section);
	void normalizeProgramIndices();

public:

	ProgramManager();

	void setMaxProgramLength(uint32_t maxProgramLength);
	int32_t getMaxProgramLength();

	void setMaxFPS(uint16_t targetFPS);
	void pause(bool blackout = true, bool fade = true);
	void unpause();
	void togglePause();

	void nudge(int32_t);

	void selectProgramCode(uint16_t programCode, bool keepPalette = false);
	void selectProgram(uint8_t programID, bool keepPalette = false);
	void selectRandomProgram();
	void nextProgram(bool keepPalette = false);
	void prevProgram(bool keepPalette = false);
	void randomizeProgramOrder();

	void addLightProgram(LightProgram &program, uint16_t sections, uint8_t modes[], uint8_t modeCount);
	void addLightProgram(LightProgram &program, uint8_t modes[], uint8_t modeCount);
	void addLightProgram(LightProgram &program, uint16_t sections);
	void addLightProgram(LightProgram &program);

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