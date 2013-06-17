#include "LSLightProgramManager.h"
#include "LSLPFadeDown.h"

pcolor_palette_t create_color_palette(pcolor_palette_factory_func factory, void *config) {
	pcolor_palette_t ret = (pcolor_palette_t)calloc(1, sizeof(color_palette_t));

	ret->factory = factory;
	ret->config = config;

	return ret;
}

plight_program_t create_light_program(plight_program_factory_func factory) {
	plight_program_t ret = (plight_program_t)calloc(1, sizeof(light_program_t));

	ret->factory = factory;

	LSLightProgram *tempProgram = factory(NULL, NULL, NULL);
	ret->programID = tempProgram->getProgramID();
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
	: maxProgramLength(-1), maxLightPrograms(maxLightPrograms), msPerFrame(20),
	maxColorPalettes(maxColorPalettes), maxLightSections(maxLightSections),
	paletteIndex(0), paletteCount(0), programIndex(0), programCount(0), programListLength(0),
	sectionCount(0), zxSound(NULL), paused(false)
{
	//Serial.print(F("\n\n   -=-   Light String Booting   -=-\n\n"));

	// TODO: This will not be enough for all the programs and their modes
	programList = (uint16_t *)calloc(maxLightPrograms * MAX_MODES, sizeof(uint16_t));
	colorPalettes = (pcolor_palette_t *)calloc(maxColorPalettes, sizeof(pcolor_palette_t));
	lightSections = (plight_section_t *)calloc(maxLightSections, sizeof(plight_section_t));
	lightStrips = (LSLEDStrip **)calloc(maxLightSections, sizeof(LSLEDStrip *));
	lightPrograms = (plight_program_t *)calloc(maxLightPrograms, sizeof(plight_program_t));

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

// ------------------------ Manager Control ------------------------

void LSLightProgramManager::setMaxProgramLength(uint32_t maxProgramLength) {
	this->maxProgramLength = maxProgramLength;
}

void LSLightProgramManager::setMaxFPS(uint16_t targetFPS) {
	msPerFrame = 1000 / targetFPS;
}

void LSLightProgramManager::setZXSoundPin(int pin) {
	if (zxSound)
		delete zxSound;

	zxSound = new LSZXSound(pin);
}

void LSLightProgramManager::fadeDown() {
	uint8_t steps = FADE_STEPS;
	uint16_t totalTime = FADE_TIME;
	uint16_t delayStep = totalTime / steps;

	for (int step = 0; step < steps; step++) {
		float ratio = (float)(steps - (step + 1)) / (float)(steps - step);

		for (int i = 0; i < sectionCount; i++) {
			LSPixelBuffer *pixelBuffer = lightSections[i]->activeProgram->getPixelBuffer();
			pixelBuffer->fade(ratio);
		}

		for (int i = 0; lightStrips[i] && (i < sectionCount); i++)
			lightStrips[i]->update();

		delay(delayStep);
	}

	delay(200);
}

void LSLightProgramManager::pause(bool blackout, bool fade) {
	if (paused) return;
	
	paused = true;
	pauseStartedAt = millis();
	
	if (blackout) {
		if (fade) {
			this->selectProgram(FADE_DOWN);
		} else {
			for (int i = 0; i < sectionCount; i++) {
				LSPixelBuffer *pixelBuffer = lightSections[i]->activeProgram->getPixelBuffer();
				pixelBuffer->clear();
			}
		}
	}
}

void LSLightProgramManager::unpause() {
	if (!paused) return;

	paused = false;
	uint32_t timeDelta = millis() - pauseStartedAt;
	for (int i = 0; i < sectionCount; i++) {
		lightSections[i]->programStartedAt += timeDelta;
	}
}

void LSLightProgramManager::togglePause() {
	paused ? unpause() : pause();
}

void LSLightProgramManager::nudge(int32_t data) {
	for (int i = 0; i < sectionCount; i++) {
		lightSections[i]->activeProgram->nudge(data);
	}
}

// ------------------------ Palette Management ------------------------

void LSLightProgramManager::selectPaletteForSection(uint8_t index, plight_section_t section) {
	if (section->colorPalette)
		delete section->colorPalette;

	Serial.print("Palette Index: ");
	Serial.print(index);
	index = paletteOrder[index]; // Respect the reodered palette
	Serial.print(" - ");
	Serial.println(index);

	section->colorPalette = colorPalettes[index]->factory();
	if (colorPalettes[index]->config)
		section->colorPalette->setConfig(colorPalettes[index]->config);

	section->colorPalette->setColorFunc(section->activeProgram->getColorFunc());
	
	if (section->colorPalette->usePaletteTable()) {
		section->colorPalette->generatePaletteTable();
	}
	
	section->pixelBuffer->setColorPalette(section->colorPalette);
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

	colorPalettes[paletteCount] = create_color_palette(factory, config);
	paletteOrder[paletteCount] = paletteCount++;
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
	Serial.println((programID << 8) | programMode, HEX);

	if (section->activeProgram) {
		delete section->activeProgram;
	}

	if (createProgram) {
		plight_program_t program = getProgramForSection(programID, section);
		section->activeProgram = program->factory(section->pixelBuffer, section->colorPalette, section->colorFunc);
	}

	Serial.print("Running: ");
	Serial.println((section->activeProgram->getProgramID() << 8) | programMode, HEX);
	
	if (!section->activeProgram->usePreviousPalette()) {
		section->paletteIndexOffset++;
		uint16_t paletteIndex = (paletteIndex + section->paletteIndexOffset) % paletteCount;
		this->selectPaletteForSection(paletteIndex, section);
	}

	section->activeProgram->setupMode(programMode);
	section->programStartedAt = millis();

	if (zxSound)
		section->activeProgram->setZXSound(zxSound);
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
}

void LSLightProgramManager::selectProgram(uint8_t programID) {
	this->selectProgramCode(programID << 8);
}

void LSLightProgramManager::nextProgramForSection(plight_section_t section) {
	uint8_t programID;
	uint8_t mode;

	if (section->activeProgram->getNextProgramCode() == 0) {
		Serial.println("Using next program in list.");

		section->programIndexOffset++;
		uint8_t index = programOrder[(programIndex + section->programIndexOffset) % programListLength];

		uint16_t programCode = programList[index];

		programID = (uint8_t)(programCode >> 8);
		mode = ((uint8_t)programCode & 0xff);
	} else {
		Serial.println("Using next program specified by program.");

		uint16_t programCode = section->activeProgram->getNextProgramCode();

		programID = (uint8_t)(programCode >> 8);
		mode = ((uint8_t)programCode & 0xff);
	}

	Serial.print("Selecting: ");
	Serial.print(programID, HEX);
	Serial.print(" - ");
	Serial.println(mode, HEX);
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
	programIndex %= programListLength;
}

void LSLightProgramManager::nextProgram() {
	this->normalizeProgramIndices();
	
	programIndex++;
	programIndex %= programListLength;
	
	uint8_t index = programOrder[programIndex];

	this->selectProgramCode(programList[index]);
}

void LSLightProgramManager::prevProgram() {
	this->normalizeProgramIndices();
	
	if (programIndex == 0)
		programIndex = programListLength - 1;
	else
		programIndex--;
	programIndex %= programListLength;

	uint8_t index = programOrder[programIndex];

	this->selectProgramCode(programList[index]);
}

void LSLightProgramManager::addLightProgram(plight_program_factory_func factory, uint16_t sections, uint8_t modes[], uint8_t modeCount) {
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
		}
	}

	lightPrograms[programCount] = program;
	programOrder[programCount] = programCount++;
	LSLightProgram *tempProgram = program->factory(NULL, NULL, NULL);

	if (!tempProgram->hideFromProgramList()) {
		for (int i = 0; i < modeCount; i++) {
			Serial.print("Adding program code to list: ");
			Serial.println((program->programID << 8) | modes[i], HEX);

			programList[programListLength++] = (program->programID << 8) | modes[i];
		}
	}

	delete tempProgram;
}

void LSLightProgramManager::addLightProgram(plight_program_factory_func factory, uint8_t modes[], uint8_t modeCount) {
	addLightProgram(factory, ALL_SECTIONS, modes, modeCount);
}

void LSLightProgramManager::addLightProgram(plight_program_factory_func factory, uint16_t sections) {
	LSLightProgram *tempProgram = factory(NULL, NULL, NULL);
	uint16_t modeCount = tempProgram->getModeCount();
	uint8_t *modes = (uint8_t *)calloc(modeCount, sizeof(uint8_t));
	
	for (int i = 0; i < modeCount; i++) modes[i] = i;
	
	delete tempProgram;

	addLightProgram(factory, sections, modes, modeCount);
	delete modes;
}

void LSLightProgramManager::addLightProgram(plight_program_factory_func factory) {
	addLightProgram(factory, ALL_SECTIONS);
}

// -------------------- Adding Sections -------------------

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

// -------------------- Primary Manager Loop -------------------

void LSLightProgramManager::loop() {
	uint32_t time = millis(), timeDelta = time - lastTime;
	lastTime = time;

	if (zxSound)
		zxSound->update();
	
	// TODO: Frame rate limiting isn't working quite right
	if (timeDelta < msPerFrame) {
		delay(msPerFrame - timeDelta);
	}

	for (int i = 0; i < sectionCount; i++) {
		LSLightProgram *program = lightSections[i]->activeProgram;
		uint32_t sectionTimeDelta = time - lightSections[i]->programStartedAt;

		if (program->isProgramFinished() || 
			(program->getProgramLength() > 0 && sectionTimeDelta > program->getProgramLength()) ||
			(maxProgramLength > 0 && sectionTimeDelta > maxProgramLength))
		{
			if (!paused)
				nextProgramForSection(lightSections[i]);

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