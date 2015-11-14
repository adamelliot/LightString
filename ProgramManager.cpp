#include "ProgramManager.h"
#include "palette.h"

ProgramManager::ProgramManager()
	: maxProgramLength(-1), msPerFrame(20),
	programIndex(0), programCount(0), programListLength(0),
	sectionCount(0), paused(false),
	brightness(255), adjustedBrightness(255), targetBrightness(255), brightnessStep(0)
{}

// ------------------------ Manager Control ------------------------

void ProgramManager::setMaxProgramLength(uint32_t maxProgramLength) {
	this->maxProgramLength = maxProgramLength;
}

int32_t ProgramManager::getMaxProgramLength() {
	return this->maxProgramLength;
}

void ProgramManager::setMaxFPS(uint16_t targetFPS) {
	msPerFrame = 1000 / targetFPS;
}

void ProgramManager::pause(bool blackout, bool fade) {
	if (paused) return;

	paused = true;
	pauseStartedAt = millis();
	
#ifdef VERBOSE
	Serial.println(F("Pause"));
#endif

	if (blackout) {
		if (fade) {
			this->fadeDown();
		} else {
			// TODO: Hard fade to black instead of wipe
			for (int i = 0; i < sectionCount; i++) {
				PixelBuffer *pixelBuffer = lightSections[i].activeProgram->getPixelBuffer();
				pixelBuffer->clear();
			}
		}
	}
}

void ProgramManager::unpause() {
	if (!paused) return;

	this->fadeUp();

	paused = false;
	uint32_t timeDelta = millis() - pauseStartedAt;
	for (int i = 0; i < sectionCount; i++) {
		lightSections[i].programStartedAt += timeDelta;
	}
}

void ProgramManager::togglePause() {
	paused ? unpause() : pause();
}

void ProgramManager::nudge(int32_t data) {
	for (int i = 0; i < sectionCount; i++) {
		lightSections[i].activeProgram->nudge(data);
	}
}

// ------------------------ Program Management ------------------------

LightProgram *ProgramManager::getProgram(uint8_t programID) {
	for (int i = 0; i < programCount; i++) {
		if (lightPrograms[i]->getProgramID() == programID)
			return lightPrograms[i];
	}

	return NULL;
}

LightProgram *ProgramManager::getProgramForSection(uint8_t programID, LightSection &section) {
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

void ProgramManager::selectProgramForSection(LightSection &section, uint8_t programID, uint8_t programMode, bool keepPalette) {
#ifdef VERBOSE
	if (programID != 0x1) {
		Serial.print(F("Selecting Program Code: "));
		Serial.println((programID << 8) | programMode, HEX);
	}
#endif

	LightProgram *program = getProgramForSection(programID, section);
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

void ProgramManager::selectProgramGroup(uint8_t programID) {
	
}

void ProgramManager::selectProgramCode(uint16_t programCode, bool keepPalette) {
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

void ProgramManager::selectProgram(uint8_t programID, bool keepPalette) {
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

void ProgramManager::selectRandomProgram() {
	programIndex = random(programListLength);
	this->nextProgram(true);

	programIndex++;
	programIndex %= programListLength;
}

void ProgramManager::nextProgramForSection(LightSection &section) {
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

void ProgramManager::normalizeProgramIndices() {
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

void ProgramManager::nextProgram(bool keepPalette) {
	this->normalizeProgramIndices();

	programIndex++;
	programIndex %= programListLength;
	
	uint8_t index = programOrder[programIndex];

	this->selectProgramCode(programList[index], keepPalette);
}

void ProgramManager::prevProgram(bool keepPalette) {
	this->normalizeProgramIndices();
	
	if (programIndex == 0)
		programIndex = programListLength - 1;
	else
		programIndex--;
	programIndex %= programListLength;

	uint8_t index = programOrder[programIndex];

	this->selectProgramCode(programList[index], keepPalette);
}

void ProgramManager::addLightProgram(LightProgram &program, uint16_t sections, uint8_t modes[], uint8_t modeCount) {
	uint8_t programID = program.getProgramID();

	for (int i = 0; i < programCount; i++) {
		if (lightPrograms[i]->getProgramID() == programID) {
			return;
		}
	}

	for (int i = 0; i < sectionCount; i++) {
		if (((1 << i) | sections) == sections) {
			LightSection &section = lightSections[i];
			section.supportedPrograms[section.programCount++] = programID;
		}
	}

	lightPrograms[programCount] = &program;
	programCount++;

	if (!program.hideFromProgramList()) {
		for (int i = 0; i < modeCount; i++) {
#ifdef VERBOSE
			Serial.print(F("Adding Program Code: "));
			Serial.print((programID << 8) | modes[i], HEX);
			Serial.print(" at: ");
			Serial.println((uint32_t)&program);
#endif

			programList[programListLength] = (programID << 8) | modes[i];
			programOrder[programListLength] = programListLength++;
		}
	}
}

void ProgramManager::addLightProgram(LightProgram &program, uint8_t modes[], uint8_t modeCount) {
	addLightProgram(program, ALL_SECTIONS, modes, modeCount);
}

void ProgramManager::addLightProgram(LightProgram &program, uint16_t sections) {
	uint8_t modeCount = program.getModeCount();
	uint8_t modes[MAX_MODES];
	
	for (int i = 0; i < modeCount; i++) modes[i] = i;

	addLightProgram(program, sections, modes, modeCount);
}

void ProgramManager::addLightProgram(LightProgram &program) {
	addLightProgram(program, ALL_SECTIONS);
}

void ProgramManager::randomizeProgramOrder() {
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

LightSection *ProgramManager::getLightSection(uint8_t index) {
	if (index >= sectionCount) return NULL;

	return &lightSections[index];
}

uint16_t ProgramManager::addLightSection(CRGB *pixels, uint16_t length) {
	if (sectionCount >= MAX_LIGHT_SECTIONS) {
#ifdef VERBOSE
		Serial.println(F("ERROR: Maximum amount of light sections already added."));
#endif
		return 0xffff;
	}

	lightSections[sectionCount] = LightSection(pixels, length);
	sectionCount++;
	
	return 1 << (sectionCount - 1);
}

// -------------------- Brightness Control -------------------

void ProgramManager::fadeDown() {
	targetBrightness = 0;
	brightnessStep = -adjustedBrightness / kTransitionFrames;
}

void ProgramManager::fadeUp(bool forceZero) {
	if (forceZero) {
		adjustedBrightness = 0;
		FastLED.setBrightness(adjustedBrightness);
	}

	targetBrightness = brightness;
	brightnessStep = (brightness - adjustedBrightness) / kTransitionFrames;
}

void ProgramManager::setBrightness(uint8_t brightness) {
	this->brightness = brightness;
	this->adjustedBrightness = brightness;
	this->targetBrightness = brightness;

	FastLED.setBrightness(brightness);
}

bool ProgramManager::isTransitioning() {
	return adjustedBrightness != targetBrightness;
}

void ProgramManager::transitionBrightness() {
	if (!this->isTransitioning()) return;

	adjustedBrightness += brightnessStep;

	if (abs(targetBrightness - adjustedBrightness) < abs(brightnessStep)) {
		adjustedBrightness = targetBrightness;
	}

	FastLED.setBrightness(adjustedBrightness);
}

// -------------------- Primary Manager Loop -------------------

void ProgramManager::finishProgramForSection(LightSection &section, uint32_t timeDelta) {
	if (section.transitionState == STARTING) {
		if (section.activeProgram->getTransition() == FADE_DOWN) {
			// Serial.println("Fade Down");
			this->fadeDown();
		}

		section.transitionState = RUNNING;
	}

	switch (section.activeProgram->getTransition()) {
		case OVERWRITE:
		section.transitionState = FINISHED;
		break;

		case WIPE:
		section.pixelBuffer.clear();
		section.transitionState = FINISHED;
		break;

		case FADE_DOWN:
		if (!this->isTransitioning()) {
			section.pixelBuffer.clear();
			section.transitionState = FINISHED;
			this->setBrightness(this->brightness);
		} else {
			section.activeProgram->update(timeDelta);
		}
		break;
	}
}

void ProgramManager::loop() {
	uint32_t time = millis(), timeDelta = time - lastTime;
	lastTime = time;

	// TODO: Frame rate limiting isn't working quite right
	if (timeDelta < msPerFrame) {
		delay(msPerFrame - timeDelta);
		
		// return;
	}

	this->transitionBrightness();

	for (int i = 0; i < sectionCount; i++) {
		LightProgram *program = lightSections[i].activeProgram;
		uint32_t sectionTimeDelta = time - lightSections[i].programStartedAt;

		if (program->isProgramFinished() || 
			(program->getProgramLength() > 0 && sectionTimeDelta > program->getProgramLength()) ||
			(maxProgramLength > 0 && sectionTimeDelta > maxProgramLength))
		{
			if (lightSections[i].transitionState == NONE) {
				lightSections[i].transitionState = STARTING;
			}

			this->finishProgramForSection(lightSections[i], timeDelta);

			if (lightSections[i].transitionState == FINISHED && !paused) {
				lightSections[i].transitionState = NONE;
				nextProgramForSection(lightSections[i]);
			}

		} else {
			program->update(timeDelta);
		}
	}
}
