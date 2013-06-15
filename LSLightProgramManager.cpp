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

	LSLightProgram *tempProgram = factory(NULL, NULL, NULL);
	ret->programID = tempProgram->getProgramID();
	ret->programModes = tempProgram->getModeCount();
	delete tempProgram;

	return ret;
}

plight_section_t create_light_section(pcolor_func colorFunc, LSLEDStrip *lightStrip, uint16_t length, uint16_t offset, uint8_t maxLightPrograms) {
	plight_section_t ret = (plight_section_t)calloc(1, sizeof(light_section_t));

	ret->colorFunc = colorFunc;
	ret->supportedPrograms = (uint8_t *)calloc(maxLightPrograms, sizeof(uint8_t));
	ret->pixelBuffer = lightStrip->getPixelBuffer(length, offset);

	return ret;
}

void free_light_section(plight_section_t section) {
	free(section->supportedPrograms);
	free(section);
}

LSLightProgramManager::LSLightProgramManager(uint8_t maxLightPrograms, uint8_t maxColorPalettes, uint8_t maxLightSections)
	: maxProgramLength(-1), maxLightPrograms(maxLightPrograms), 
	maxColorPalettes(maxColorPalettes), maxLightSections(maxLightSections),
	paletteIndex(0), paletteCount(0), programIndex(0), programCount(0), programListLength(0),
	sectionCount(0), zxSound(NULL)
{
	//Serial.print(F("\n\n   -=-   Light String Booting   -=-\n\n"));

	// TODO: This will not be enough for all the programs and their modes
	programList = (uint16_t *)calloc(maxLightPrograms, sizeof(uint16_t));
	colorPalettes = (pcolor_palette_t *)calloc(maxColorPalettes, sizeof(pcolor_palette_t));
	lightSections = (plight_section_t *)calloc(maxLightSections, sizeof(plight_section_t));
	lightStrips = (LSLEDStrip **)calloc(maxLightSections, sizeof(LSLEDStrip *));
	lightPrograms = (plight_program_t *)calloc(maxLightPrograms, sizeof(plight_program_t));

	paletteOrder = (uint8_t *)calloc(maxColorPalettes, sizeof(uint8_t));
	programOrder = (uint8_t *)calloc(maxLightPrograms, sizeof(uint8_t));

	loadState();
}

void LSLightProgramManager::setMaxProgramLength(uint32_t maxProgramLength) {
	this->maxProgramLength = maxProgramLength;
}

void LSLightProgramManager::saveState() {
	// TODO: Write the palette and program to EPROM
}

void LSLightProgramManager::loadState() {
	// TODO: Read the palette and program to EPROM
}

// ------------------------ Palette Management ------------------------

void LSLightProgramManager::selectPaletteForSection(uint8_t index, plight_section_t section) {
	if (section->colorPalette)
		delete section->colorPalette;

	index = paletteOrder[index]; // Respect the reodered palette

	section->colorPalette = colorPalettes[index]->factory();
	if (colorPalettes[index]->config)
		section->colorPalette->setConfig(colorPalettes[index]->config);

	Serial.println("Setting up palette.");
	
	Serial.println(section->colorPalette == 0);
	Serial.println(section->activeProgram == 0);
	
	delay(400);

	section->colorPalette->setColorFunc(section->activeProgram->getColorFunc());
	section->activeProgram->setColorPalette(section->colorPalette);
}

void LSLightProgramManager::selectPalette(uint8_t index) {
	for (uint8_t i = 0; i < sectionCount; i++) {
		plight_section_t section = lightSections[i];
		section->paletteIndexOffset = 0;

		this->selectPaletteForSection(paletteIndex, section);
	}
}

void LSLightProgramManager::normalizePaletteIndices() {
	uint8_t minIndex = 0xff;

	for (int i = 0; i < sectionCount; i++) {
		if (minIndex > lightSections[i]->paletteIndexOffset)
			minIndex = lightSections[i]->paletteIndexOffset;
	}

	for (int i = 0; i < sectionCount; i++) {
		lightSections[i]->paletteIndexOffset -= minIndex;
	}

	paletteIndex += minIndex;
	paletteIndex %= paletteCount;
}

// Select the next palette and set that palette for all programs
void LSLightProgramManager::nextPalette() {
	this->normalizePaletteIndices();

	paletteIndex++;
	paletteIndex %= paletteCount;

	this->selectPalette(paletteIndex);
}

void LSLightProgramManager::prevPalette() {
	this->normalizePaletteIndices();

	if (paletteIndex == 0)
		paletteIndex = paletteCount - 1;
	else
		paletteIndex--;
	paletteIndex %= paletteCount;

	this->selectPalette(paletteIndex);
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
}

// ------------------------ Program Management ------------------------

plight_program_t LSLightProgramManager::getProgram(uint8_t programID) {
	for (int i = 0; i < programCount; i++) {
		if (lightPrograms[i]->programID == programID)
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

	// Program isn't supported so find the next program in the queue that is
	for (int i = 0; i < programListLength; i++) {
		programID = (uint8_t)(programList[(i + 1 + programIndex) % programListLength] >> 8);

		for (int i = 0; i < programCount; i++) {
			if (section->supportedPrograms[i] == programID)
				return this->getProgram(programID);
		}
	}

	Serial.println(F("ERROR: Could not find suitable program for section."));

	return NULL;
}

void LSLightProgramManager::selectProgramForSection(plight_section_t section, uint8_t programID, uint8_t programMode) {
	bool createProgram = true;

	Serial.print("Selecting section program: ");
	Serial.println(programID, HEX);

	if (section->activeProgram) {
		if (section->activeProgram->getMode() == programMode) {
			// Programs themselves will get the setMode call and handle if they should reset or not
			createProgram = false;
		} else {
			delete section->activeProgram;
		}
	}

	Serial.print("Going to create a program: ");
	Serial.println(createProgram);

	if (createProgram) {
		plight_program_t program = getProgramForSection(programID, section);
		section->activeProgram = program->factory(section->pixelBuffer, section->colorPalette, section->colorFunc);
	}

	Serial.print("Active ProgramID: ");
	Serial.println(section->activeProgram->getProgramID(), HEX);
	
	if (!section->activeProgram->usePreviousPalette()) {
		section->paletteIndexOffset++;
		uint16_t paletteIndex = (paletteIndex + section->paletteIndexOffset) % paletteCount;
		this->selectPaletteForSection(paletteIndex, section);
	}

	section->activeProgram->setupMode(programMode);
	section->programStartedAt = millis();

	Serial.println("Mode was setup.");
	delay(400);
	/*
	if (zxSound)
		section->activeProgram->setZXSound(zxSound);*/
}

void LSLightProgramManager::selectProgramGroup(uint8_t programID) {
	
}

void LSLightProgramManager::selectProgramCode(uint16_t programCode) {
	uint8_t programID = (uint8_t)(programCode >> 8);
	uint8_t mode = ((uint8_t)programCode & 0xff);

	for (int i = 0; i < sectionCount; i++) {
		lightSections[i]->programIndexOffset = 0;
	}

	Serial.print("Selecting Program Code: ");
	Serial.println(programCode, HEX);

	// Select programs for each section
	if ((programID & 0x80) == 0x80) {
		this->selectProgramGroup(programID);
	} else {
		Serial.println("Select from selectProgramCode");
		for (int i = 0; i < sectionCount; i++) {
			selectProgramForSection(lightSections[i], programID, mode);
		}
	}

	// Compute frame rate and end times
	uint8_t newFrameRate = 0;

	for (int i = 0; i < sectionCount; i++) {
		newFrameRate += lightSections[i]->activeProgram->getFrameRate();
	}

	newFrameRate /= sectionCount;
	msPerFrame = 1000 / newFrameRate;
}

void LSLightProgramManager::selectProgram(uint8_t programID) {
	this->selectProgramCode(programID << 8);
}

void LSLightProgramManager::nextProgramForSection(plight_section_t section) {
	uint8_t programID;
	uint8_t mode;

	if (section->activeProgram->getNextProgramCode() == 0) {
		section->programIndexOffset++;
		uint8_t index = programOrder[(programIndex + section->programIndexOffset) % programListLength];

		uint16_t programCode = programList[index];

		programID = (uint8_t)(programCode >> 8);
		mode = ((uint8_t)programCode & 0xff);
	} else {
		uint16_t programCode = section->activeProgram->getNextProgramCode();

		programID = (uint8_t)(programCode >> 8);
		mode = ((uint8_t)programCode & 0xff);
	}

	Serial.println("Select from nextProgramForSection");
	delay(400);
	this->selectProgramForSection(section, programID, mode);
}

void LSLightProgramManager::normalizeProgramIndices() {
	uint8_t minIndex = 0xff;

	for (int i = 0; i < sectionCount; i++) {
		if (minIndex > lightSections[i]->programIndexOffset)
			minIndex = lightSections[i]->programIndexOffset;
	}

	for (int i = 0; i < sectionCount; i++) {
		lightSections[i]->programIndexOffset -= minIndex;
	}

	programIndex += minIndex;
	programIndex %= paletteCount;
}

void LSLightProgramManager::nextProgram() {
	this->normalizeProgramIndices();
	
	uint8_t index = programOrder[programIndex++];
	programIndex %= programListLength;

	this->selectProgramCode(programList[index]);
}

void LSLightProgramManager::prevProgram() {
	this->normalizeProgramIndices();
	
	if (programIndex == 0)
		programIndex = programListLength - 1;
	else
		programIndex--;

	uint8_t index = programOrder[programIndex];
	programIndex %= programListLength;

	this->selectProgramCode(programList[index]);
}

void LSLightProgramManager::addLightProgram(plight_program_factory_func factory, uint16_t sections) {
	plight_program_t program = create_light_program(factory);

	for (int i = 0; i < programCount; i++)
		if (lightPrograms[i]->programID == program->programID) {
			free(program);
			return;
		}

	for (int i = 0; i < sectionCount; i++) {
		if (((1 << i) | sections) == sections) {
			plight_section_t section = lightSections[i];
			section->supportedPrograms[section->programCount++] = program->programID;
			
			Serial.print("Added program to: ");
			Serial.println(i);
		}
	}

	lightPrograms[programCount++] = program;
	
	for (uint8_t i = 0; i < program->programModes; i++) {
		programList[programListLength++] = (program->programID << 8) | i;
	}
}

// -------------------- Adding Sections & Sound -------------------

uint16_t LSLightProgramManager::addLightSection(pcolor_func colorFunc, LSLEDStrip *lightStrip, uint16_t length, uint16_t offset) {
	if (sectionCount >= maxLightSections) {
		Serial.println(F("ERROR: Maximum amount of light sections already added."));
		return 0xffff;
	}

	if (length == 0) length = lightStrip->getLength();
	lightSections[sectionCount] = create_light_section(colorFunc, lightStrip, length, offset, maxLightPrograms);

	int i = 0;
	do {
		if (lightStrips[i] == lightStrip)
			break;
		if (!lightStrips[i]) {
			lightStrips[i] = lightStrip;
			break;
		}
	} while (i < maxLightSections && lightStrips[i++]);

	sectionCount++;
	
	return 1 << (sectionCount - 1);
}

void LSLightProgramManager::setZXSoundPin(int pin) {
	if (zxSound)
		delete zxSound;

	zxSound = new LSZXSound(pin);
}

// -------------------- Primary Manager Loop -------------------

void LSLightProgramManager::loop() {
	uint32_t time = millis(), timeDelta = time - lastTime;
	lastTime = time;

	if (zxSound)
		zxSound->update();

	if (timeDelta < msPerFrame)
		delay(msPerFrame - timeDelta);

	for (int i = 0; i < sectionCount; i++) {
		LSLightProgram *program = lightSections[i]->activeProgram;
		uint32_t sectionTimeDelta = time - lightSections[i]->programStartedAt;

		if (program->isProgramFinished() || 
			(program->getProgramLength() > 0 && sectionTimeDelta > program->getProgramLength()) ||
			(maxProgramLength > 0 && sectionTimeDelta > maxProgramLength))
		{
			Serial.print("Loading next for section: ");
			Serial.println(i);
			//nextProgramForSection(lightSections[i]);
		} else {
			program->update();
		}
	}

	for (int i = 0; lightStrips[i] && (i < sectionCount); i++) {
		lightStrips[i]->update();
	}
}

// --- Old Activate ---

/*
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
*/
