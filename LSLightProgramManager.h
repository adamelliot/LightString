#ifndef _LSLIGHTPROGRAMMANAGER_H_
#define _LSLIGHTPROGRAMMANAGER_H_

#include "LSLightProgram.h"
#include "LSLEDStrip.h"

#define ALL_SECTIONS 0xffff
#define MAX_MODES 8

typedef LSLightProgram *(*plight_program_factory_func)(LSPixelBuffer *);

typedef struct light_program_s light_program_t, *plight_program_t;
typedef struct light_section_s light_section_t, *plight_section_t;

plight_program_t create_light_program(plight_program_factory_func factory);
plight_section_t create_light_section(LSLEDStrip *lightStrip, uint16_t length, uint16_t offset, uint8_t maxLightPrograms);
void free_light_section(plight_section_t section);

struct light_program_s {
	plight_program_factory_func factory;
	uint8_t programID;
};

// TODO: Implement program grouping
// NOTE: Group Program IDs are >= 0x80
struct light_program_group_s {
	uint8_t programID;
	// Pairs of codes and strip addreses
	uint32_t *programCodes;
};

struct light_section_s {
	uint8_t *supportedPrograms;
	uint8_t programCount;

	int16_t programIndexOffset;

	LSPixelBuffer *pixelBuffer;
	LSLightProgram *activeProgram;
	
	uint32_t programStartedAt;
};

class LSLightProgramManager {
private:
	bool verbose;

	uint8_t maxLightSections;
	LSLEDStrip **lightStrips;
	plight_section_t *lightSections;
	uint8_t sectionCount;

	uint8_t maxLightPrograms;
	plight_program_t *lightPrograms;
	uint16_t *programList;
	uint8_t programListLength;
	uint8_t programCount;
	uint8_t *programOrder; // Order of the program list
	uint8_t programIndex; // Index in the program order

	// Program Manager Timing
	int32_t maxProgramLength;
	uint32_t lastTime;
	uint16_t msPerFrame;

	bool paused;
	uint32_t pauseStartedAt;

	plight_program_t getProgram(uint8_t programID);
	plight_program_t getProgramForSection(uint8_t programID, plight_section_t section);
	void selectProgramForSection(plight_section_t section, uint8_t programID, uint8_t programMode);
	void selectProgramGroup(uint8_t programID);
	void nextProgramForSection(plight_section_t section);
	void normalizeProgramIndices();

public:

	LSLightProgramManager(uint8_t maxLightPrograms = 8, uint8_t maxLightSections = 1, bool verbose = false);

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

	void addLightProgram(plight_program_factory_func factory, uint16_t sections, uint8_t modes[], uint8_t modeCount);
	void addLightProgram(plight_program_factory_func factory, uint8_t modes[], uint8_t modeCount);
	void addLightProgram(plight_program_factory_func factory, uint16_t sections);
	void addLightProgram(plight_program_factory_func factory);

	plight_section_t getLightSection(uint8_t);
	uint16_t addLightSection(LSLEDStrip *lightStrip, uint16_t length = 0, uint16_t offset = 0);
	
	void loop();
};

#endif