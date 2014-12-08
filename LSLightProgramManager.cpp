#include "LSLightProgramManager.h"
#include "LSLPFadeDown.h"

LSLightProgramManager::LSLightProgramManager()
	: maxProgramLength(-1), msPerFrame(20),
	programIndex(0), programCount(0), programListLength(0),
	sectionCount(0), paused(false), verbose(verbose), brightness(255)
{}

void LSLightProgramManager::setup() {
/*	if (verbose) {
		Serial.begin(9600);
		Serial.print(F("\n\n   -=-   Light String Booting   -=-\n\n"));
	}*/
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
	
#ifdef VERBOSE
	Serial.println(F("Pause"));
#endif

	if (blackout) {
		if (fade) {
			selectProgram(FADE_DOWN, false);
		} else {
			for (int i = 0; i < sectionCount; i++) {
				LSPixelBuffer *pixelBuffer = lightSections[i].activeProgram->getPixelBuffer();
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
		lightSections[i].programStartedAt += timeDelta;
	}
}

void LSLightProgramManager::togglePause() {
	paused ? unpause() : pause();
}

void LSLightProgramManager::nudge(int32_t data) {
	for (int i = 0; i < sectionCount; i++) {
		lightSections[i].activeProgram->nudge(data);
	}
}

// ------------------------ Program Management ------------------------

LSLightProgram *LSLightProgramManager::getProgram(uint8_t programID) {
	for (int i = 0; i < programCount; i++) {
		if (lightPrograms[i]->getProgramID() == programID)
			return lightPrograms[i];
	}

	return NULL;
}

LSLightProgram *LSLightProgramManager::getProgramForSection(uint8_t programID, light_section_t &section) {
	// Program is supported
	for (int i = 0; i < programCount; i++) {
		if (section.supportedPrograms[i] == programID)
			return this->getProgram(programID);
	}

	// Program isn't supported so find the next program in the queue that is
	for (int i = 0; i < programListLength; i++) {
		programID = (uint8_t)(programList[(i + 1 + programIndex) % programListLength] >> 8);

		for (int i = 0; i < programCount; i++) {
			if (section.supportedPrograms[i] == programID)
				return this->getProgram(programID);
		}
	}

#ifdef VERBOSE
	Serial.println(F("ERROR: Could not find suitable program for section."));
#endif

	return NULL;
}

void LSLightProgramManager::selectProgramForSection(light_section_t &section, uint8_t programID, uint8_t programMode, bool keepPalette) {
#ifdef VERBOSE
	if (verbose && programID != 0x1) {
		Serial.print(F("Selecting Program Code: "));
		Serial.println((programID << 8) | programMode, HEX);
	}
#endif

	LSLightProgram *program = getProgramForSection(programID, section);
	section.activeProgram = program;
	section.activeProgram->setPixelBuffer(&section.pixelBuffer);

	if (!keepPalette) {
		sectionsChanged++;
		if ((sectionsChanged % sectionCount) == 0) {
			Palettes.next();
		}
	}
	
	section.activeProgram->setupMode(programMode);
	section.programStartedAt = millis();
}

void LSLightProgramManager::selectProgramGroup(uint8_t programID) {
	
}

void LSLightProgramManager::selectProgramCode(uint16_t programCode, bool keepPalette) {
	uint8_t programID = (uint8_t)(programCode >> 8);
	uint8_t mode = ((uint8_t)programCode & 0xff);

	for (int i = 0; i < sectionCount; i++) {
		lightSections[i].programIndexOffset = 0;
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

void LSLightProgramManager::nextProgramForSection(light_section_t &section) {
	uint8_t programID;
	uint8_t mode;

	if (section.activeProgram->getNextProgramCode() == 0) {
		uint8_t index = programOrder[(programIndex + section.programIndexOffset) % programListLength];
		section.programIndexOffset++;

		uint16_t programCode = programList[index];

		programID = (uint8_t)(programCode >> 8);
		mode = ((uint8_t)programCode & 0xff);
	} else {

		uint16_t programCode = section.activeProgram->getNextProgramCode();

		programID = (uint8_t)(programCode >> 8);
		mode = ((uint8_t)programCode & 0xff);
	}

	this->selectProgramForSection(section, programID, mode, section.activeProgram->usePreviousPalette());
}

void LSLightProgramManager::normalizeProgramIndices() {
	uint8_t minIndex = 0xff;

	for (int i = 0; i < sectionCount; i++) {
		if (minIndex > lightSections[i].programIndexOffset)
			minIndex = lightSections[i].programIndexOffset;
	}

	for (int i = 0; i < sectionCount; i++) {
		lightSections[i].programIndexOffset -= minIndex;
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

void LSLightProgramManager::addLightProgram(LSLightProgram &program, uint16_t sections, uint8_t modes[], uint8_t modeCount) {
	uint8_t programID = program.getProgramID();

	for (int i = 0; i < programCount; i++) {
		if (lightPrograms[i]->getProgramID() == programID) {
			return;
		}
	}

	for (int i = 0; i < sectionCount; i++) {
		if (((1 << i) | sections) == sections) {
			light_section_t &section = lightSections[i];
			section.supportedPrograms[section.programCount++] = programID;
		}
	}

	lightPrograms[programCount] = &program;
	programCount++;

	if (!program.hideFromProgramList()) {
		for (int i = 0; i < modeCount; i++) {
#ifdef VERBOSE
			Serial.print(F("Adding Program Code: "));
			Serial.println((programID << 8) | modes[i], HEX);
			LOG("Added Program At: ", (uint16_t)&program);
#endif

			programList[programListLength] = (programID << 8) | modes[i];
			programOrder[programListLength] = programListLength++;
		}
	}
}

void LSLightProgramManager::addLightProgram(LSLightProgram &program, uint8_t modes[], uint8_t modeCount) {
	addLightProgram(program, ALL_SECTIONS, modes, modeCount);
}

void LSLightProgramManager::addLightProgram(LSLightProgram &program, uint16_t sections) {
	uint8_t modeCount = program.getModeCount();
	uint8_t modes[MAX_MODES];
	
	for (int i = 0; i < modeCount; i++) modes[i] = i;

	addLightProgram(program, sections, modes, modeCount);
}

void LSLightProgramManager::addLightProgram(LSLightProgram &program) {
	addLightProgram(program, ALL_SECTIONS);
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

// -------------------- Adding Sections -------------------

plight_section_t LSLightProgramManager::getLightSection(uint8_t index) {
	if (index >= sectionCount) return NULL;

	return &lightSections[index];
}

uint16_t LSLightProgramManager::addLightSection(CRGB *pixels, uint16_t length) {
	if (sectionCount >= MAX_LIGHT_SECTIONS) {
#ifdef VERBOSE
		Serial.println(F("ERROR: Maximum amount of light sections already added."));
#endif
		return 0xffff;
	}

	lightSections[sectionCount] = light_section_t();
	lightSections[sectionCount].pixelBuffer.setPixels(pixels, length);
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
		LSLightProgram *program = lightSections[i].activeProgram;
		uint32_t sectionTimeDelta = time - lightSections[i].programStartedAt;

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
