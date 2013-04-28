#ifndef _LSLIGHTPROGRAMMANAGER_H_
#define _LSLIGHTPROGRAMMANAGER_H_

#include "LSLightProgram.h"
#include "LSLEDStrip.h"
#include "LSZXSound.h"

#define ALL_STRIPS 0xff
#define ANY_LIGHT_PROGRAM 0xff

#define FADE_STEPS 30
#define FADE_TIME 1000

#define DEFAULT_PROGRAM_LENGTH 10000
#define DEFAULT_MAX_PROGRAMS 8
#define DEFAULT_MAX_PALETTES 16
#define DEFAULT_MAX_LIGHT_SECTIONS 2

typedef LSLightProgram *(*plight_program_factory_func)(LSPixelBuffer *, LSColorPalette *, pcolor_func);

typedef struct color_palette_s color_palette_t, *pcolor_palette_t;
typedef struct light_program_s light_program_t, *plight_program_t;
typedef struct light_section_s light_section_t, *plight_section_t;

pcolor_palette_t create_color_palette(pcolor_palette_factory_func factory, void *config);
plight_program_t create_light_program(plight_program_factory_func factory, void *config);
plight_section_t create_light_section(pcolor_func colorFunc, LSLEDStrip *lightStrip, uint16_t length, uint16_t offset, uint8_t maxLightPrograms);
void free_light_section(plight_section_t section);

struct color_palette_s {
	pcolor_palette_factory_func factory;
	void *config;
};

struct light_program_s {
	plight_program_factory_func factory;
	void *config;
};

struct light_section_s {
	pcolor_func colorFunc;
	plight_program_t *supportedPrograms;
	LSPixelBuffer *pixelBuffer;
	LSLightProgram *activeProgram;
};

class LSLightProgramManager {
private:
	LSZXSound *zxSound;
	
	uint8_t maxLightPrograms;
	uint8_t maxColorPalettes;
	uint8_t maxLightSections;
	
	uint8_t *programs;
	pcolor_palette_t *colorPalettes;
	plight_section_t *lightSections;
	LSLEDStrip **lightStrips;
	
	uint32_t programLength;
	
	uint8_t programsCount;
	uint8_t palettesCount;
	uint8_t lightSectionsCount;
	
	uint8_t *paletteOrder;
	uint8_t paletteIndex;
	
	uint8_t *programOrder;
	uint8_t activeProgramID;
	
	LSColorPalette *activeColorPalette;
	
	uint32_t lastTime;
	uint32_t programSwitchAfter;
	uint16_t msPerFrame;
	
	void randomizePaletteOrder();

	void saveState();
	void loadState();
	/*
	void pulse();*/
	void fadeDown();

public:
	
	LSLightProgramManager(uint32_t programLength = DEFAULT_PROGRAM_LENGTH, uint8_t maxLightPrograms = 8, uint8_t maxColorPalettes = 16, uint8_t maxLightSections = 2);

	uint8_t addLightSection(pcolor_func colorFunc, LSLEDStrip *lightStrip, uint16_t length = 0, uint16_t offset = 0);
	void addLightProgram(uint8_t strips, uint8_t id, plight_program_factory_func factory, void *config = NULL);
	void addColorPalette(pcolor_palette_factory_func factory, void *config = NULL);

	void selectNewProgram(uint8_t id = ANY_LIGHT_PROGRAM);
	
	void setZXSoundPin(int pin);
	
	void loop();
};

#endif