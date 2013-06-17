#ifndef _LSLIGHTPROGRAMMANAGER_H_
#define _LSLIGHTPROGRAMMANAGER_H_

#include "LSLightProgram.h"
#include "LSLEDStrip.h"
#include "LSZXSound.h"

#define ALL_SECTIONS 0xffff
#define MAX_MODES 8

#define FADE_STEPS 30
#define FADE_TIME 1000

typedef LSLightProgram *(*plight_program_factory_func)(LSPixelBuffer *, LSColorPalette *, pcolor_func);

typedef struct color_palette_s color_palette_t, *pcolor_palette_t;
typedef struct light_program_s light_program_t, *plight_program_t;
typedef struct light_section_s light_section_t, *plight_section_t;

pcolor_palette_t create_color_palette(pcolor_palette_factory_func factory, void *config);
plight_program_t create_light_program(plight_program_factory_func factory);
plight_section_t create_light_section(pcolor_func colorFunc, LSLEDStrip *lightStrip, uint16_t length, uint16_t offset, uint8_t maxLightPrograms);
void free_light_section(plight_section_t section);

struct color_palette_s {
	pcolor_palette_factory_func factory;
	void *config;
};

struct light_program_s {
	plight_program_factory_func factory;
	uint8_t programID;
};

struct light_program_group_s {
	uint8_t programID;
	// Pairs of codes and strip addreses
	uint32_t *programCodes;
};

struct light_section_s {
	pcolor_func colorFunc;
	uint8_t *supportedPrograms;
	uint8_t programCount;

	int16_t programIndexOffset;
	int16_t paletteIndexOffset;
	
	LSPixelBuffer *pixelBuffer;
	LSColorPalette *colorPalette;
	LSLightProgram *activeProgram;
	
	uint32_t programStartedAt;
};

class LSLightProgramManager {
private:
	LSZXSound *zxSound;
	
	uint8_t maxColorPalettes;
	pcolor_palette_t *colorPalettes;
	uint8_t paletteCount;
	uint8_t *paletteOrder;
	uint8_t paletteIndex;
	
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
	
	void saveState();
	void loadState();

	void selectPaletteForSection(uint8_t index, plight_section_t section);
	void selectPalette(uint8_t index);
	void normalizePaletteIndices();

	plight_program_t getProgram(uint8_t programID);
	plight_program_t getProgramForSection(uint8_t programID, plight_section_t section);
	void selectProgramForSection(plight_section_t section, uint8_t programID, uint8_t programMode, bool keepPalette = false);
	void selectProgramGroup(uint8_t programID);
	void nextProgramForSection(plight_section_t section);
	void normalizeProgramIndices();
	
//	void fadeDown();

public:
	
	LSLightProgramManager(uint8_t maxLightPrograms = 8, uint8_t maxColorPalettes = 16, uint8_t maxLightSections = 1);
	
	void setMaxProgramLength(uint32_t maxProgramLength);
	void setMaxFPS(uint16_t targetFPS);
	void setZXSoundPin(int pin);
	void pause(bool blackout = true, bool fade = true);
	void unpause();
	void togglePause();
	
	void nudge(int32_t);
	
	void nextPalette();
	void prevPalette();
	void randomizePaletteOrder();
	void addColorPalette(pcolor_palette_factory_func factory, void *config = NULL);

	void selectProgramCode(uint16_t programCode);
	void selectProgram(uint8_t programID);
	void nextProgram();
	void prevProgram();
	void randomizeProgramOrder();

	void addLightProgram(plight_program_factory_func factory, uint16_t sections, uint8_t modes[], uint8_t modeCount);
	void addLightProgram(plight_program_factory_func factory, uint8_t modes[], uint8_t modeCount);
	void addLightProgram(plight_program_factory_func factory, uint16_t sections);
	void addLightProgram(plight_program_factory_func factory);

	uint16_t addLightSection(pcolor_func colorFunc, LSLEDStrip *lightStrip, uint16_t length = 0, uint16_t offset = 0);
	
	void loop();
};

#endif