#include "LSColorPalette.h"

void printColor(CRGB col) {
	Serial.print("(");
	Serial.print(col.r);
	Serial.print(", ");
	Serial.print(col.g);
	Serial.print(", ");
	Serial.print(col.b);
	Serial.print(")");
}

LSColorPalette::LSColorPalette() : paletteTable(NULL)
{}

LSColorPalette::~LSColorPalette() {
	freePaletteTable();
}

uint8_t LSColorPalette::getFlags() {
	return flags;
}

bool LSColorPalette::isMirrored() {
	return (flags | PALETTE_MIRRORED) == flags;
}

CRGB *LSColorPalette::generatePaletteTable() {
	if (!paletteTable)
		paletteTable = (CRGB *)calloc(PALETTE_SIZE, 3);
	
	for (int i = 0; i < PALETTE_SIZE; i++) {
		paletteTable[i] = this->getTableColor(i);
	}

	return paletteTable;
}

void LSColorPalette::freePaletteTable() {
	if (paletteTable) free(paletteTable);
	paletteTable = NULL;
}

CRGB *LSColorPalette::getPaletteTable() { return paletteTable; }

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

CRGB LSColorPalette::getColor(uint8_t index) {
	return CRGB(index, index, index);
}

CRGB LSColorPalette::getTableColor(uint8_t index) {
	return getColor(index);
}
