#ifndef _LIGHTPROGRAMMANAGER_H_
#define _LIGHTPROGRAMMANAGER_H_

#include "FastLED.h"
#include "LightProgram.h"

#define ALL_SECTIONS 0xffff
#define MAX_MODES 7
#define MAX_LIGHT_PROGRAMS 10
#define MAX_LIGHT_SECTIONS 1

#define VERBOSE

namespace LightString {

typedef struct light_section_s light_section_t, *plight_section_t;

// TODO: Implement program grouping
// NOTE: Group Program IDs are >= 0x80
struct light_program_group_s {
	uint8_t programID;
	// Pairs of codes and strip addreses
	uint32_t *programCodes;
};

struct light_section_s {
	uint8_t supportedPrograms[MAX_LIGHT_PROGRAMS];
	uint8_t programCount;

	int16_t programIndexOffset;

	PixelBuffer pixelBuffer;
	LightProgram *activeProgram;
	
	uint32_t programStartedAt;
	
	inline light_section_s() 
		: programCount(0), programIndexOffset(0), programStartedAt(0), activeProgram(0) {
	}
};

class ProgramManager {
private:
	uint8_t brightness;

	light_section_t lightSections[MAX_LIGHT_SECTIONS];
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

	LightProgram *getProgram(uint8_t programID);
	LightProgram *getProgramForSection(uint8_t programID, light_section_t &section);
	void selectProgramForSection(light_section_t &section, uint8_t programID, uint8_t programMode, bool keepPalette = false);
	void selectProgramGroup(uint8_t programID);
	void nextProgramForSection(light_section_t &section);
	void normalizeProgramIndices();

public:

	ProgramManager();
	void setup();

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

	plight_section_t getLightSection(uint8_t);
	uint16_t addLightSection(CRGB *pixels, uint16_t length);
	
	void loop();
};

};

#endif