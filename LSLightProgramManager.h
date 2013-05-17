#ifndef _LSLIGHTPROGRAMMANAGER_H_
#define _LSLIGHTPROGRAMMANAGER_H_

#include "LSLightProgram.h"
#include "LSLEDStrip.h"
#include "LSZXSound.h"

#define ALL_STRIPS 0xff
#define ANY_LIGHT_PROGRAM 0xff

#define DEFAULT_PROGRAM_LENGTH 10000
#define DEFAULT_MAX_PROGRAMS 8
#define DEFAULT_MAX_QUEUE_SIZE 20
#define DEFAULT_MAX_PALETTES 16
#define DEFAULT_MAX_LIGHT_SECTIONS 2

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
	uint8_t programIndex;
	LSPixelBuffer *pixelBuffer;
	LSColorPalette *colorPalette;
	LSLightProgram *activeProgram;

	bool loadNext;
};

class LSLightProgramManager {
private:
	LSZXSound *zxSound;
	
	uint8_t maxLightPrograms;
	uint8_t maxColorPalettes;
	uint8_t maxLightSections;
	
	pcolor_palette_t *colorPalettes;
	plight_section_t *lightSections;
	plight_program_t *lightPrograms;
	uint16_t *programQueue;

	LSLEDStrip **lightStrips;
	
	uint32_t programLength;
	
	uint8_t paletteCount;
	uint8_t sectionCount;
	uint8_t programCount;
	uint8_t programQueueLength;
	
	uint8_t *paletteOrder;
	uint16_t paletteIndex;
	
	uint8_t *programOrder;
	uint16_t programIndex;

	LSColorPalette *activeColorPalette;
	
	uint32_t lastTime;
	uint32_t programSwitchAfter;
	uint16_t msPerFrame;
	
	void saveState();
	void loadState();

	void selectPalette(uint8_t index);
	void selectProgram(uint16_t programCode);

	plight_program_t getProgram(uint8_t programID);
	void activateProgramForSection(plight_section_t section, uint8_t programID, uint8_t programMode);

public:
	
	LSLightProgramManager(uint32_t programLength = DEFAULT_PROGRAM_LENGTH, uint8_t maxLightPrograms = 8, uint8_t maxColorPalettes = 16, uint8_t maxLightSections = 2);

	uint8_t addLightSection(pcolor_func colorFunc, LSLEDStrip *lightStrip, uint16_t length = 0, uint16_t offset = 0);
	void addLightProgram(plight_program_factory_func factory);
	void addColorPalette(pcolor_palette_factory_func factory, void *config = NULL);

	void randomizePaletteOrder();
	void randomizeProgramOrder();

	void nextPalette();
	void prevPalette();
	
	void selectProgram(uint16_t programCode);
	void nextPalette();
	void prevPalette();
	
	void setZXSoundPin(int pin);
	
	void loop();
};

#endif