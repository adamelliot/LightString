#ifndef _LSLIGHTPROGRAMMANAGER_H_
#define _LSLIGHTPROGRAMMANAGER_H_

#include "FastLED.h"
#include "LSLightProgram.h"

#define ALL_SECTIONS 0xffff
#define MAX_MODES 8
#define MAX_LIGHT_PROGRAMS 6
#define MAX_LIGHT_SECTIONS 1

#define VERBOSE

typedef LSLightProgram *(*plight_program_factory_func)(LSPixelBuffer *);

#define LOG(MSG, VAL) {Serial.print(F(MSG " ")); Serial.println(VAL); }
#define LOG2(MSG, V1, V2) {Serial.print(F(MSG " ")); Serial.print(V1); Serial.print(F(", ")); Serial.println(V2); }

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

	LSPixelBuffer pixelBuffer;
	LSLightProgram *activeProgram;
	
	uint32_t programStartedAt;
	
	inline light_section_s() 
		: programCount(0), programIndexOffset(0), programStartedAt(0), activeProgram(0) {
	}
};

class LSLightProgramManager {
private:
	bool verbose;
	
	uint8_t brightness;

	light_section_t lightSections[MAX_LIGHT_SECTIONS];
	uint8_t sectionCount;
	
	uint8_t sectionsChanged;

	LSLightProgram *lightPrograms[MAX_LIGHT_PROGRAMS];
	uint16_t programList[MAX_LIGHT_PROGRAMS * MAX_MODES];
	uint8_t programListLength;
	uint8_t programCount;
	uint8_t programOrder[MAX_LIGHT_PROGRAMS]; // Order of the program list
	uint8_t programIndex; // Index in the program order

	// Program Manager Timing
	int32_t maxProgramLength;
	uint32_t lastTime;
	uint16_t msPerFrame;

	bool paused;
	uint32_t pauseStartedAt;

	LSLightProgram *getProgram(uint8_t programID);
	LSLightProgram *getProgramForSection(uint8_t programID, light_section_t &section);
	void selectProgramForSection(light_section_t &section, uint8_t programID, uint8_t programMode, bool keepPalette = false);
	void selectProgramGroup(uint8_t programID);
	void nextProgramForSection(light_section_t &section);
	void normalizeProgramIndices();

public:

	LSLightProgramManager();
	void setup();

	void setMaxProgramLength(uint32_t maxProgramLength);
	void setMaxFPS(uint16_t targetFPS);
	void pause(bool blackout = true, bool fade = true);
	void unpause();
	void togglePause();
	
	void nudge(int32_t);
	
	void selectProgramCode(uint16_t programCode, bool keepPalette = false);
	void selectProgram(uint8_t programID, bool keepPalette = false);
	void selectRandomProgram();
	void nextProgram();
	void prevProgram();
	void randomizeProgramOrder();

	void addLightProgram(LSLightProgram &program, uint16_t sections, uint8_t modes[], uint8_t modeCount);
	void addLightProgram(LSLightProgram &program, uint8_t modes[], uint8_t modeCount);
	void addLightProgram(LSLightProgram &program, uint16_t sections);
	void addLightProgram(LSLightProgram &program);

	plight_section_t getLightSection(uint8_t);
	uint16_t addLightSection(CRGB *pixels, uint16_t length);
	
	void loop();
};

#endif