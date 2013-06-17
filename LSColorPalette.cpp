#include "LSColorPalette.h"

color_t colorRGB(uint8_t r, uint8_t g, uint8_t b) {
	return (color_t){r, g, b};
}

color_t colorRBG(uint8_t r, uint8_t g, uint8_t b) {
	return (color_t){r, b, g};
}

color_t colorBGR(uint8_t r, uint8_t g, uint8_t b) {
	return (color_t){b, g, r};
}

color_t colorGRB(uint8_t r, uint8_t g, uint8_t b) {
	return (color_t){g, r, b};
}

color_t randomColor() {
	return (color_t){random(255), random(255), random(255)};
}

color_t whiteColor() {
	return (color_t){255, 255, 255};
}

color_t blackColor() {
	return (color_t){0, 0, 0};
}

LSColorPalette::LSColorPalette() 
 : colorFunc(colorRGB), paletteTable(NULL)
{}

LSColorPalette::~LSColorPalette() {
	freePaletteTable();
}

void LSColorPalette::setColorFunc(pcolor_func colorFunc) {
	this->colorFunc = colorFunc;
}

pcolor_func LSColorPalette::getColorFunc() {
	return colorFunc;
}

uint8_t LSColorPalette::getFlags() {
	return flags;
}

bool LSColorPalette::isMirrored() {
	return (flags | PALETTE_MIRRORED) == flags;
}

pcolor_t LSColorPalette::generatePaletteTable() {
	if (!paletteTable)
		paletteTable = (pcolor_t)calloc(PALETTE_SIZE, 3);
	
	for (int i = 0; i < PALETTE_SIZE; i++) {
		paletteTable[i] = this->getTableColor(i);
	}

	return paletteTable;
}

void LSColorPalette::freePaletteTable() {
	if (paletteTable) free(paletteTable);
	paletteTable = NULL;
}

pcolor_t LSColorPalette::getPaletteTable() { return paletteTable; }

/**
 * Fast fade doesn't use FP, just shift, only allows 8 steps.
 */
void LSColorPalette::fade(uint8_t shift) {
	if (!paletteTable) return;

	for (int i = 0; i < PALETTE_SIZE * 3; i++) {
		((uint8_t *)paletteTable)[i] >>= shift;
	}
}

void LSColorPalette::fade(float ratio) {
	if (!paletteTable) return;

	for (int i = 0; i < PALETTE_SIZE * 3; i++) {
		((uint8_t *)paletteTable)[i] *= ratio;
	}
}

bool LSColorPalette::usePaletteTable() {
	return false;
}

void LSColorPalette::setConfig(void *config) {}
void *LSColorPalette::getConfig() { return NULL; }

color_t LSColorPalette::getColor(uint8_t index) {
	return (color_t){index, index, index};
}

color_t LSColorPalette::getTableColor(uint8_t index) {
	return getColor(index);
}
