#include "LSLightProgramManager.h"
#include "LSLPFadeDown.h"

plight_program_t create_light_program(plight_program_factory_func factory) {
	plight_program_t ret = (plight_program_t)calloc(1, sizeof(light_program_t));

	ret->factory = factory;

	LSLightProgram *tempProgram = factory(NULL);
	ret->programID = tempProgram->getProgramID();
	delete tempProgram;

	return ret;
}

plight_section_t create_light_section(CRGB *pixels, uint16_t length, uint8_t maxLightPrograms) {
	plight_section_t ret = (plight_section_t)calloc(1, sizeof(light_section_t));

	ret->supportedPrograms = (uint8_t *)calloc(maxLightPrograms, sizeof(uint8_t));
	ret->pixelBuffer = new LSPixelBuffer(pixels, length);
	ret->pixelBuffer->clear();

	return ret;
}

void free_light_section(plight_section_t section) {
	free(section->supportedPrograms);
	free(section);
}

LSLightProgramManager::LSLightProgramManager(uint8_t maxLightPrograms, uint8_t maxLightSections, bool verbose)
	: maxProgramLength(-1), maxLightPrograms(maxLightPrograms), msPerFrame(20),
	maxLightSections(maxLightSections),
	programIndex(0), programCount(0), programListLength(0),
	sectionCount(0), paused(false), verbose(verbose), brightness(255)
{}

void LSLightProgramManager::setup() {
	if (verbose) {
		Serial.begin(9600);
		delay(2000);
		Serial.print(F("\n\n   -=-   Light String Booting   -=-\n\n"));
	}

	// TODO: This will not be enough for all the programs and their modes
	programList = (uint16_t *)calloc(maxLightPrograms * MAX_MODES, sizeof(uint16_t));
	LOG2("programList: ", (uint16_t)programList, maxLightPrograms * MAX_MODES * sizeof(uint16_t));
	
	lightSections = (plight_section_t *)calloc(maxLightSections, sizeof(plight_section_t));
	lightPrograms = (plight_program_t *)calloc(maxLightPrograms, sizeof(plight_program_t));

	programOrder = (uint8_t *)calloc(maxLightPrograms, sizeof(uint8_t));
}

// ------------------------ Manager Control ------------------------

void LSLightProgramManager::setMaxProgramLength(uint32_t maxProgramLength) {
	this->maxProgramLength = maxProgramLength;
}

void LSLightProgramManager::setMaxFPS(uint16_t targetFPS) {
	msPerFrame = 1000 / targetFPS;
}

void LSLightProgramManager::pause(bool blackout, bool fade) {
	if (paused) return;

	paused = true;
	pauseStartedAt = millis();
	
	if (verbose) Serial.println("Pause");

	if (blackout) {
		if (fade) {
			selectProgram(FADE_DOWN, false);
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
	LOG("programCount: ", programCount);
	for (int i = 0; i < programCount; i++) {
		if (section->supportedPrograms[i] == programID)
			return this->getProgram(programID);
	}

	// Program isn't supported so find the next program in the queue that is
	LOG("programListLength: ", programListLength);
	for (int i = 0; i < programListLength; i++) {
		programID = (uint8_t)(programList[(i + 1 + programIndex) % programListLength] >> 8);

		for (int i = 0; i < programCount; i++) {
			if (section->supportedPrograms[i] == programID)
				return this->getProgram(programID);
		}
	}

	if (verbose) Serial.println(F("ERROR: Could not find suitable program for section."));

	return NULL;
}

void LSLightProgramManager::selectProgramForSection(plight_section_t section, uint8_t programID, uint8_t programMode, bool keepPalette) {
	bool createProgram = true;

	if (verbose && programID != 0x1) {
		Serial.print(F("Selecting Program Code: "));
		Serial.println((programID << 8) | programMode, HEX);
	}

	if (section->activeProgram) {
		LOG("About to free: ", (uint16_t)section->activeProgram);
		free(section->activeProgram);
	}

	delay(500);
	Serial.println("Freed");

	if (createProgram) {
		plight_program_t program = getProgramForSection(programID, section);
		section->activeProgram = program->factory(section->pixelBuffer);
	}

	if (!keepPalette) {
		sectionsChanged++;
		if ((sectionsChanged % sectionCount) == 0) {
			Palettes.next();
		}
	}

	section->activeProgram->setupMode(programMode);
	section->programStartedAt = millis();
}

void LSLightProgramManager::selectProgramGroup(uint8_t programID) {
	
}

void LSLightProgramManager::selectProgramCode(uint16_t programCode, bool keepPalette) {
	uint8_t programID = (uint8_t)(programCode >> 8);
	uint8_t mode = ((uint8_t)programCode & 0xff);

	for (int i = 0; i < sectionCount; i++) {
		lightSections[i]->programIndexOffset = 0;
	}

	for (int i = 0; i < programListLength; i++) {
		if (programList[programOrder[i]] == programCode) {
			programIndex = i;
			break;
		}
	}

	if (!keepPalette) {
		sectionsChanged = 0;
		Palettes.next();
	}

	// Select programs for each section
	if ((programID & 0x80) == 0x80) {
		this->selectProgramGroup(programID);
	} else {
		for (int i = 0; i < sectionCount; i++) {
			Serial.println("selectProgramCode");
			this->selectProgramForSection(lightSections[i], programID, mode, true);
		}
	}
}

void LSLightProgramManager::selectProgram(uint8_t programID, bool keepPalette) {
	uint16_t code = programID << 8;
	
	for (int i = 0; i < programListLength; i++) {
		if (programList[programOrder[i]] == code) {
			programIndex = i;
			break;
		}
	}

	this->selectProgramCode(code, keepPalette);

	programIndex++;
	programIndex %= programListLength;
}

void LSLightProgramManager::selectRandomProgram() {
	programIndex = random(programListLength);
	this->nextProgram();

	programIndex++;
	programIndex %= programListLength;
}

void LSLightProgramManager::nextProgramForSection(plight_section_t section) {
	uint8_t programID;
	uint8_t mode;

	if (section->activeProgram->getNextProgramCode() == 0) {
		//Serial.println(F("Using next program in list."));
		
		uint8_t index = programOrder[(programIndex + section->programIndexOffset) % programListLength];
		section->programIndexOffset++;

		LOG("index: ", index);
		LOG("programList: ", (uint16_t)programList);

		uint16_t programCode = programList[index];
		LOG("programCode 1: ", programCode);

		programID = (uint8_t)(programCode >> 8);
		mode = ((uint8_t)programCode & 0xff);
	} else {
		//Serial.println(F("Using next program specified by program."));

		uint16_t programCode = section->activeProgram->getNextProgramCode();
		LOG("programCode 2: ", programCode);

		programID = (uint8_t)(programCode >> 8);
		mode = ((uint8_t)programCode & 0xff);
	}

	Serial.println("\nnextProgramForSection");
	LOG("section: ", (uint16_t)section);
	LOG("programID: ", programID);
	LOG("mode: ", mode);
	delay(200);
	this->selectProgramForSection(section, programID, mode, section->activeProgram->usePreviousPalette());
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


// TODO: This may not work quite right
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

	lightPrograms[programCount++] = program;
	LSLightProgram *tempProgram = program->factory(NULL);

	if (!tempProgram->hideFromProgramList()) {
		for (int i = 0; i < modeCount; i++) {
			if (verbose) {
				Serial.print(F("Adding Program Code: "));
				Serial.println((program->programID << 8) | modes[i], HEX);
			}

			programList[programListLength] = (program->programID << 8) | modes[i];
			programOrder[programListLength] = programListLength++;
		}
	}

	delete tempProgram;
}

void LSLightProgramManager::addLightProgram(plight_program_factory_func factory, uint8_t modes[], uint8_t modeCount) {
	addLightProgram(factory, ALL_SECTIONS, modes, modeCount);
}

void LSLightProgramManager::randomizeProgramOrder() {
	for (int i = 0; i < programListLength; i++)
		programOrder[i] = 0xff;

	uint8_t programIndex;
	for (int i = 0; i < programListLength; i++) {
		while (programOrder[programIndex = random(programListLength)] != 0xff);
		programOrder[programIndex] = i;
	}
	/*
	Serial.print("Order: ");
	for (int i = 0; i < programListLength; i++) {
		Serial.print(programOrder[i]);
		Serial.print("-");
		Serial.print(programList[programOrder[i]], HEX);
		Serial.print(", ");
	}
	Serial.println();*/
}

void LSLightProgramManager::addLightProgram(plight_program_factory_func factory, uint16_t sections) {
	LSLightProgram *tempProgram = factory(NULL);
	uint8_t modeCount = tempProgram->getModeCount();
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

plight_section_t LSLightProgramManager::getLightSection(uint8_t index) {
	if (index >= sectionCount) return NULL;

	return lightSections[index];
}

uint16_t LSLightProgramManager::addLightSection(CRGB *pixels, uint16_t length) {
	if (sectionCount >= maxLightSections) {
		if (verbose) Serial.println(F("ERROR: Maximum amount of light sections already added."));
		return 0xffff;
	}

	lightSections[sectionCount] = create_light_section(pixels, length, maxLightPrograms);
	sectionCount++;
	
	return 1 << (sectionCount - 1);
}

// -------------------- Primary Manager Loop -------------------

void LSLightProgramManager::loop() {
	uint32_t time = millis(), timeDelta = time - lastTime;
	lastTime = time;

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
			program->update(timeDelta);
		}
	}

	FastLED.show();
}
