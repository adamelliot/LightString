#include "EEPROM.h"
#include "palette.h"

using namespace LightString;

PaletteManager Palettes;

void printColor(CRGB col) {
	Serial.print("(");
	Serial.print(col.r);
	Serial.print(",");
	Serial.print(col.g);
	Serial.print(",");
	Serial.print(col.b);
	Serial.println(")");
}

PaletteManager::PaletteManager()
	: writePalettes(false), customPalettes(0), staticOffset(0), currentPalette(CRGBPalette())
{
	staticPalettes = EEPROM.read(0);

	if (staticPalettes == 255) staticPalettes = 0;

	if (staticPalettes > 0) {
		paletteIndex = 0;
		loadPalette(0);
	}
}

void PaletteManager::updateEEPROM() {
	staticPalettes = 0;
	writePalettes = true;
}

void PaletteManager::writePalette(uint16_t offset, CRGBPalette &palette) {
	uint8_t len = palette.size * CHANNEL_SIZE;
	uint8_t *colors = (uint8_t *)palette.colors;

	offset += EEPROM_OFFSET;
	
	EEPROM.write(offset++, len);
	
	for (int i = 0; i < len; i++) {
		EEPROM.write(offset++, colors[i]);
	}
}

CRGBPalette PaletteManager::readPalette(uint16_t offset) {
	uint8_t len;
	uint8_t colors[MAX_PALETTE_COLORS * sizeof(CRGB)];

	offset += EEPROM_OFFSET;
	len = EEPROM.read(offset++);

	for (int i = 0; i < len; i++) {
		colors[i] = EEPROM.read(offset++);
	}

	CRGBPalette newPalette = CRGBPalette((len / 3), (CRGB *)colors);

	return newPalette;
}

void PaletteManager::loadPalette(uint8_t index) {
	uint16_t offset = EEPROM_OFFSET;

	for (int i = 0; i < index; i++) {
		uint8_t len = EEPROM.read(offset);
		offset += len + 1;
	}

	currentPalette = readPalette(offset - EEPROM_OFFSET);
}

uint8_t PaletteManager::paletteCount() {
	return staticPalettes + customPalettes;
}

void PaletteManager::setCustomPalettes(uint8_t customPalettes, bool reset) {
	this->customPalettes = customPalettes;

	if (reset) {
		// TODO: Write simple palettes for each custom one
	}
}

CRGB PaletteManager::getColor(uint8_t index) {
	return currentPalette[index];
}

void PaletteManager::randomizeOrder() {
	paletteIndex = random(paletteCount());
	loadPalette(paletteIndex);
}

void PaletteManager::add(CRGBPalette palette) {
	if (!writePalettes) return;
	
	writePalette(staticOffset, palette);
	staticOffset += 1 + (palette.size * CHANNEL_SIZE);

	if (staticPalettes == 0) {
		currentPalette = palette;
	}

	staticPalettes++;
	EEPROM.write(0, staticPalettes);
}

void PaletteManager::setActive(CRGBPalette &palette) {
	currentPalette = palette;
}

void PaletteManager::next() {
	paletteIndex++;
	paletteIndex %= paletteCount();

	loadPalette(paletteIndex);
}

void PaletteManager::previous() {
	if (paletteIndex == 0) {
		paletteIndex = paletteCount() - 1;
	} else {
		paletteIndex--;
	}
	
	loadPalette(paletteIndex);
}
