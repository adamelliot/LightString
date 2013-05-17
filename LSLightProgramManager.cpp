#include "LSLightProgramManager.h"

pcolor_palette_t create_color_palette(pcolor_palette_factory_func factory, void *config) {
	pcolor_palette_t ret = (pcolor_palette_t)calloc(1, sizeof(light_program_t));

	ret->factory = factory;
	ret->config = config;

	return ret;
}

plight_program_t create_light_program(plight_program_factory_func factory) {
	plight_program_t ret = (plight_program_t)calloc(1, sizeof(light_program_t));

	ret->factory = factory;

	LPLightProgram *tempProgram = factory(NULL, NULL, NULL);
	ret->programID = tempProgram->getProgramID();
	delete tempProgram;

	return ret;
}

plight_section_t create_light_section(pcolor_func colorFunc, LSLEDStrip *lightStrip, uint16_t length, uint16_t offset, uint8_t maxLightPrograms) {
	plight_section_t ret = (plight_section_t)calloc(1, sizeof(light_section_t));

	ret->supportedPrograms = (plight_program_t *)calloc(maxLightPrograms, sizeof(uint8_t));
	ret->progamIndex = 0;
	ret->colorFunc = colorFunc;
	ret->pixelBuffer = lightStrip->getPixelBuffer(length, offset);

	return ret;
}

void free_light_section(plight_section_t section) {
	free(section->supportedPrograms);
	free(section);
}

LSLightProgramManager::LSLightProgramManager(uint32_t programLength, uint8_t maxLightPrograms, uint8_t maxColorPalettes, uint8_t maxLightSections)
	: programLength(programLength), maxLightPrograms(maxLightPrograms), 
	maxColorPalettes(maxColorPalettes), maxLightSections(maxLightSections),
	activeProgramID(0), programCount(0), lightSectionsCount(0),
	activeColorPalette(NULL), zxSound(NULL)
{
	Serial.print(F("\n\n   -=-   Light String Booting   -=-\n\n"));

	programs = (uint8_t *)calloc(maxLightPrograms, sizeof(uint8_t));
	colorPalettes = (pcolor_palette_t *)calloc(maxColorPalettes, sizeof(pcolor_palette_t));
	lightSections = (plight_section_t *)calloc(maxLightSections, sizeof(plight_section_t));
	lightStrips = (LSLEDStrip **)calloc(maxLightSections, sizeof(LSLEDStrip *));
	lightPrograms = (plight_program_t *)calloc(maxLightPrograms, sizeof(plight_section_t));

	paletteOrder = (uint8_t *)calloc(maxColorPalettes, sizeof(uint8_t));
	programOrder = (uint8_t *)calloc(maxLightPrograms, sizeof(uint8_t));

	loadState();
}

void LSLightProgramManager::saveState() {
	// TODO: Write the palette and program to EPROM
}

void LSLightProgramManager::loadState() {
	// TODO: Read the palette and program to EPROM
}

void LSLightProgramManager::selectPalette(uint8_t index) {
	if (activeColorPalette)
		delete activeColorPalette;
	activeColorPalette = colorPalettes[index]->factory();
	if (colorPalettes[index]->config)
		activeColorPalette->setConfig(colorPalettes[index]->config);

	// TODO: Switch for all programs
}

void LSLightProgramManager::nextPalette() {
	uint8_t index = paletteOrder[paletteIndex++];
	paletteIndex %= paletteCount;

	this->selectPalette(index);
}

void LSLightProgramManager::prevPalette() {
	if (paletteIndex == 0)
		paletteIndex = paletteCount - 1;
	else
		paletteIndex--;

	uint8_t index = paletteOrder[paletteIndex];
	paletteIndex %= paletteCount;

	this->selectPalette(index);
}

void LSLightProgramManager::activateProgram(uint8_t index) {
	uint8_t lastMode;

	//Serial.println("Selecting program");

	// First time through we don't have a palette so don't fade down
//	if (activeColorPalette)
//		fadeDown();

	if (id == ANY_LIGHT_PROGRAM) {
		lastMode = activeProgramID;
		if (programCount > 1) {
			while(lastMode == activeProgramID) {
				activeProgramID = programs[random(programCount)];
			}
		}
		else
			activeProgramID = programs[0];
	} else
		activeProgramID = id;

	id = activeProgramID;

	//Serial.print("activeProgramID: ");
	//Serial.println(activeProgramID);

	nextPalette();

	//Serial.print("Color Palette Index: ");
	//Serial.println(currentColorPalette);

	uint8_t newFrameRate = 0;
	void *config = NULL;

	for (int i = 0; i < lightSectionsCount; i++) {
		if (lightSections[i]->supportedPrograms[id]) {
			if (lightSections[i]->activeProgram) {
				delete lightSections[i]->activeProgram;
			}
			lightSections[i]->activeProgram = lightSections[i]->supportedPrograms[id]->factory(lightSections[i]->pixelBuffer, activeColorPalette, lightSections[i]->colorFunc);
			lightSections[i]->pixelBuffer->setColorPalette(activeColorPalette);

			if (zxSound)
				lightSections[i]->activeProgram->setZXSound(zxSound);

			if (!config)
				config = lightSections[i]->activeProgram->getConfig();

			void *cfg = lightSections[i]->supportedPrograms[id]->config;
			if (!cfg)
				cfg = config;

			if (cfg)
				lightSections[i]->activeProgram->setConfig(cfg);

			newFrameRate += lightSections[i]->activeProgram->getFrameRate();
		}
	}

	for (int i = 0; lightStrips[i] && (i < lightSectionsCount); i++) {
		if (lightStrips[i]->useIndexedPixelBuffer()) {
			// TODO: This is going to be glitchy with divided 
			activeColorPalette->setColorFunc(lightSections[i]->activeProgram->getColorFunc());
			activeColorPalette->generatePaletteTable();
			lightStrips[i]->setColorPalette(activeColorPalette);
		}
	}

	newFrameRate /= lightSectionsCount;
	msPerFrame = 1000 / newFrameRate;
	lastTime = millis();
	//Serial.print("Program Length: ");
	//Serial.println(programLength);
	programSwitchAfter = lastTime + programLength;
}

plight_program_t LSLightProgramManager::getProgram(uint8_t programID) {
	for (int i = 0; i < programCount; i++) {
		if (lightPrograms[i].programID == programID)
			return lightPrograms[i];
	}

	return NULL;
}

plight_program_t LSLightProgramManager::getProgramForSection(uint8_t programID, plight_section_t section) {
	// Program is supported
	for (int i = 0; i < programCount; i++) {
		if (section->supportedPrograms[i] == programID)
			return this->getProgram(programID);
	}

	// Program isn't support so find the next program in the queue that is
	for (int i = 0; i < programQueueLength; i++) {
		programID = (uint8_t)(programQueue[(i + 1 + programIndex) % programQueueLength] >> 8);

		for (int i = 0; i < programCount; i++) {
			if (section->supportedPrograms[i] == programID)
				return this->getProgram(programID);
		}
	}

	Serial.println(F("ERROR: Could not find suitable program for section."));

	return NULL;
}

void LSLightProgramManager::activateProgramForSection(plight_section_t section, uint8_t programID, uint8_t programMode) {
	if (section->activeProgram) {
		if (section->activeProgram->getMode() == programMode) {
			// Force reset of the program (should we do this or let them run?)
			section->activeProgram->setMode(programMode);
			return;
		} else {
			delete section->activeProgram;
		}
	}

	plight_program_t program = getProgramForSection(programID, section);
	section->activeProgram = program->factory(section->pixelBuffer, activeColorPalette, section->colorFunc);

	if (zxSound)
		section->activeProgram->setZXSound(zxSound);
}

void LSLightProgramManager::selectProgramGroup(uint8_t programID) {
	
}

// Programs need to change all at once as the palette is shared
void LSLightProgramManager::selectProgram(uint16_t programCode) {
	uint8_t programID = (uint8_t)(programCode >> 8);
	uint8_t mode = ((uint8_t)programCode & 0xff);

	// Change palette first
	nextPalette();

	// Select programs for each section
	if ((programID & 0x80) == 0x80) {
		this->selectProgramGroup(programID);
	} else {
		for (int i = 0; i < sectionCount; i++) {
			activateProgramForSection(lightSections[i], programID, programMode);
		}
	}

	// Compute frame rate and end times
	uint8_t newFrameRate = 0;

	for (int i = 0; i < sectionCount; i++) {
		newFrameRate += lightSections[i]->activeProgram->getFrameRate();
	}

	newFrameRate /= sectionCount;
	msPerFrame = 1000 / newFrameRate;
	lastTime = millis();
	//Serial.print("Program Length: ");
	//Serial.println(programLength);
	programSwitchAfter = lastTime + programLength;
}

void LSLightProgramManager::nextProgram() {
	uint8_t index = programOrder[programIndex++];
	programIndex %= programQueueLength;

	this->selectProgram((programQueue[index] << 8) | 0xff);
}

void LSLightProgramManager::prevProgram() {
	if (programIndex == 0)
		programIndex = programQueueLength - 1;
	else
		programIndex--;

	uint8_t index = programOrder[programIndex];
	programIndex %= programQueueLength;

	this->selectProgram((programQueue[index] << 8) | 0xff);
}

// -------------------- Adding Programs, Sections & Palettes -------------------

uint8_t LSLightProgramManager::addLightSection(pcolor_func colorFunc, LSLEDStrip *lightStrip, uint16_t length, uint16_t offset) {
	if (lightSectionsCount >= maxLightSections) {
		Serial.println(F("ERROR: Maximum amount of light sections already added."));
		return 0xff;
	}

	if (length == 0) length = lightStrip->getLength();
	lightSections[lightSectionsCount] = create_light_section(colorFunc, lightStrip, length, offset, maxLightPrograms);

	int i = 0;
	do {
		if (lightStrips[i] == lightStrip)
			break;
		if (!lightStrips[i]) {
			lightStrips[i] = lightStrip;
			break;
		}
	} while (i < maxLightSections && lightStrips[i++]);

	lightSectionsCount++;

	return 1 << (lightSectionsCount - 1);
}

void LSLightProgramManager::addLightProgram(plight_program_factory_func factory) {
	for (int i = 0; i < lightSectionsCount; i++) {
		if (((1 << i) | strips) == strips) {
			lightSections[i]->supportedPrograms[id] = create_light_program(factory);
		}
	}

	for (int i = 0; i < programCount; i++)
		if (programs[i] == id)
			return;

	programs[programCount++] = id;
}

void LSLightProgramManager::randomizePaletteOrder() {
	for (int i = 0; i < paletteCount; i++)
		paletteOrder[i] = maxColorPalettes;

	uint8_t paletteIndex;
	for (int i = 0; i < paletteCount; i++) {
		while (paletteOrder[paletteIndex = random(paletteCount)] != maxColorPalettes);
		paletteOrder[paletteIndex] = i;
	}
}

void LSLightProgramManager::addColorPalette(pcolor_palette_factory_func factory, void *config) {
	if (paletteCount >= maxColorPalettes) {
		Serial.println(F("ERROR: Already loaded the maximum amount of palettes."));
		return;
	}

	colorPalettes[paletteCount++] = create_color_palette(factory, config);
	randomizePaletteOrder();
}

void LSLightProgramManager::setZXSoundPin(int pin) {
	if (zxSound)
		delete zxSound;

	zxSound = new LSZXSound(pin);
}

void LSLightProgramManager::loop() {
	uint32_t time = millis(), timeDelta = time - lastTime;
	lastTime = time;

	if (zxSound)
		zxSound->update();

	if (programSwitchAfter <= time) {
		selectNewProgram();
		return;
	}

	if (timeDelta < msPerFrame)
		delay(msPerFrame - timeDelta);

	for (int i = 0; i < lightSectionsCount; i++) {
		activeColorPalette->setColorFunc(lightSections[i]->activeProgram->getColorFunc());
		lightSections[i]->activeProgram->update();
	}

	for (int i = 0; lightStrips[i] && (i < lightSectionsCount); i++) {
		lightStrips[i]->update();
	}
}
