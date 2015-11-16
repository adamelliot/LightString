#include "ProgramManager.h"

PROGRAM_MANAGER_TEMPLATE
PROGRAM_MANAGER_CLASS::ProgramManager()
	: sectionCount(0), programListLength(0), programCount(0), programIndex(0),
		maxProgramLength(-1), msPerFrame(20),
		paused(false), brightness(255), targetBrightness(255), adjustedBrightness(255),
		brightnessStep(0), programEventHandler(0)
{}

// ------------------------ Manager Control ------------------------

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::pause(bool blackout, bool fade) {
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
				CRGBBuffer *pixelBuffer = lightSections[i].activeProgram->getPixelBuffer();
				pixelBuffer->clear();
			}
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::unpause() {
	if (!paused) return;

	this->fadeUp();

	paused = false;
	uint32_t timeDelta = millis() - pauseStartedAt;
	for (int i = 0; i < sectionCount; i++) {
		lightSections[i].programStartedAt += timeDelta;
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::togglePause() {
	paused ? unpause() : pause();
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::nudge(int32_t data) {
	for (int i = 0; i < sectionCount; i++) {
		lightSections[i].activeProgram->nudge(data);
	}
}

// ------------------------ Program Management ------------------------

PROGRAM_MANAGER_TEMPLATE
TLightProgram<PIXEL> *PROGRAM_MANAGER_CLASS::getProgram(ProgramCode programCode) {
	int copy = 0;
	for (int i = 0; i < programCount; i++) {
		if (lightPrograms[i]->getProgramID() == programCode.programID) {
			if (copy == programCode.copyID) {
				return lightPrograms[i];
			} else {
				copy++;
			}
		}
	}

	return NULL;
}

PROGRAM_MANAGER_TEMPLATE
TLightProgram<PIXEL> *PROGRAM_MANAGER_CLASS::getProgramForSection(ProgramCode programCode, LightSection<PIXEL, MAX_LIGHT_PROGRAMS, MAX_LAYERS> &section) {
	// Program is supported
	for (int i = 0; i < programCount; i++) {
		if (section.supportedPrograms[i] == programCode.programID) {
			return this->getProgram(programCode);
		}
	}

	// Program isn't supported so find the next program in the queue that is
	for (int i = 0; i < programListLength; i++) {
		programCode = programList[(i + 1 + programIndex) % programListLength];

		for (int i = 0; i < programCount; i++) {
			if (section.supportedPrograms[i] == programCode.programID) {
				return this->getProgram(programCode);
			}
		}
	}

#ifdef VERBOSE
	Serial.println(F("ERROR: Could not find suitable program for section."));
#endif

	return NULL;
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::selectProgramForSection(LightSection<PIXEL, MAX_LIGHT_PROGRAMS, MAX_LAYERS> &section, ProgramCode programCode) {
#ifdef VERBOSE
	Serial.print(F("Selecting Program: "));
	Serial.print((programCode.programID << 8) | programCode.mode, HEX);
	Serial.print(F(" ("));
	Serial.print(programCode.copyID, DEC);
	Serial.println(F(")"));
#endif

	if (programEventHandler) {
		// programEventHandler(*section.activeProgram, PROGRAM_FINISHED);
	}

	TLightProgram<PIXEL> *program = getProgramForSection(programCode, section);
	section.activeProgram = program;
	section.activeProgram->setPixelBuffer(section.outputBuffer);

	if (programEventHandler) {
		// programEventHandler(*section.activeProgram, PROGRAM_STARTED);
	}

	section.activeProgram->setupMode(programCode.mode);
	section.programStartedAt = millis();
}

/*
PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::selectProgramGroup(uint8_t programID) {
	
}
*/

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::selectProgramCode(ProgramCode programCode) {
	for (int i = 0; i < sectionCount; i++) {
		lightSections[i].programIndexOffset = 0;
	}

	for (int i = 0; i < programListLength; i++) {
		if (programList[programOrder[i]] == programCode) {
			programIndex = i;
			break;
		}
	}

	/* NOTE: Group check
	if ((programID & 0x80) == 0x80) {
		this->selectProgramGroup(programID);
	} else {
	*/

	// Select programs for each section
	for (int i = 0; i < sectionCount; i++) {
		this->selectProgramForSection(lightSections[i], programCode);
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::selectProgram(uint8_t programID, uint8_t copyID) {
	ProgramCode programCode(programID, copyID);

	for (int i = 0; i < programListLength; i++) {
		ProgramCode &pc = programList[programOrder[i]];
		if (pc.programID == programCode.programID && pc.copyID == programCode.programID) {
			programIndex = i;
			break;
		}
	}

	this->selectProgramCode(programCode);

	programIndex++;
	programIndex %= programListLength;
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::selectRandomProgram() {
	programIndex = random(programListLength);
	this->nextProgram();

	programIndex++;
	programIndex %= programListLength;
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::nextProgramForSection(LightSection<PIXEL, MAX_LIGHT_PROGRAMS, MAX_LAYERS> &section) {
	ProgramCode programCode;

	if (section.activeProgram->getNextProgramCode() == 0) {
		uint8_t index = programOrder[(programIndex + section.programIndexOffset) % programListLength];
		section.programIndexOffset++;

		programCode = programList[index];
	} else {
		uint16_t code = section.activeProgram->getNextProgramCode();

		programCode.programID = (uint8_t)(code >> 8);
		programCode.mode = ((uint8_t)code & 0xff);
	}

	this->selectProgramForSection(section, programCode);
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::normalizeProgramIndices() {
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

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::nextProgram() {
	this->normalizeProgramIndices();

	programIndex++;
	programIndex %= programListLength;
	
	uint8_t index = programOrder[programIndex];

	this->selectProgramCode(programList[index]);
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::prevProgram() {
	this->normalizeProgramIndices();
	
	if (programIndex == 0)
		programIndex = programListLength - 1;
	else
		programIndex--;
	programIndex %= programListLength;

	uint8_t index = programOrder[programIndex];

	this->selectProgramCode(programList[index]);
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::addLightProgram(TLightProgram<PIXEL> &program, uint16_t sections, uint8_t modes[], uint8_t modeCount) {
	uint8_t programID = program.getProgramID();

	int copyID = 0;
	for (int i = 0; i < programCount; i++) {
		if (lightPrograms[i]->getProgramID() == programID) {
			copyID++;
		}
	}

	for (int i = 0; i < sectionCount; i++) {
		if (((1 << i) | sections) == sections) {
			LightSection<PIXEL, MAX_LIGHT_PROGRAMS, MAX_LAYERS> &section = lightSections[i];
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

			programList[programListLength] = ProgramCode(programID, copyID, modes[i]);
			programOrder[programListLength] = programListLength++;
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::addLightProgram(TLightProgram<PIXEL> &program, uint8_t modes[], uint8_t modeCount) {
	addLightProgram(program, ALL_SECTIONS, modes, modeCount);
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::addLightProgram(TLightProgram<PIXEL> &program, EProgramMode programModes) {
	uint8_t modeCount = program.getModeCount();
	uint8_t modes[MAX_MODES];
	
	for (int i = 0; i < MAX_MODES; i++) {
		if (((programModes >> i) & 1) == 1) {
			modes[i] = i;
		}
	}

	addLightProgram(program, ALL_SECTIONS, modes, modeCount);
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::addLightProgram(TLightProgram<PIXEL> &program, uint16_t sections) {
	uint8_t modeCount = program.getModeCount();
	uint8_t modes[MAX_MODES];
	
	for (int i = 0; i < modeCount; i++) modes[i] = i;

	addLightProgram(program, sections, modes, modeCount);
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::addLightProgram(TLightProgram<PIXEL> &program) {
	addLightProgram(program, ALL_SECTIONS);
}

/*
PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::addLightPrograms(TLightProgram<PIXEL> programs[], uint8_t count) {
	for (int i = 0; i < count; i++) {
		addLightProgram(programs[i]);
	}
}
*/

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::randomizeProgramOrder() {
	for (int i = 0; i < programListLength; i++)
		programOrder[i] = 0xff;

	uint8_t programIndex;
	for (int i = 0; i < programListLength; i++) {
		while (programOrder[programIndex = random(programListLength)] != 0xff);
		programOrder[programIndex] = i;
	}
}

// -------------------- Adding Sections -------------------

PROGRAM_MANAGER_TEMPLATE
LightSection<PIXEL, MAX_LIGHT_PROGRAMS, MAX_LAYERS> *PROGRAM_MANAGER_CLASS::getLightSection(uint8_t index) {
	if (index >= sectionCount) return NULL;

	return &lightSections[index];
}

PROGRAM_MANAGER_TEMPLATE
uint16_t PROGRAM_MANAGER_CLASS::addLightSection(CRGBBuffer &pixelBuffer) {
	if (sectionCount >= MAX_LIGHT_SECTIONS) {
#ifdef VERBOSE
		Serial.println(F("ERROR: Maximum amount of light sections already added."));
#endif
		return 0xffff;
	}

	lightSections[sectionCount] = LightSection<PIXEL, MAX_LIGHT_PROGRAMS, MAX_LAYERS>(&pixelBuffer);
	sectionCount++;
	
	return 1 << (sectionCount - 1);
}

// -------------------- Brightness Control -------------------

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::fadeDown() {
	targetBrightness = 0;
	brightnessStep = -adjustedBrightness / kTransitionFrames;
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::fadeUp(bool forceZero) {
	if (forceZero) {
		adjustedBrightness = 0;
		FastLED.setBrightness(adjustedBrightness);
	}

	targetBrightness = brightness;
	brightnessStep = (brightness - adjustedBrightness) / kTransitionFrames;
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::setBrightness(uint8_t brightness) {
	this->brightness = brightness;
	this->adjustedBrightness = brightness;
	this->targetBrightness = brightness;

	FastLED.setBrightness(brightness);
}

PROGRAM_MANAGER_TEMPLATE
bool PROGRAM_MANAGER_CLASS::isTransitioning() {
	return adjustedBrightness != targetBrightness;
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::transitionBrightness() {
	if (!this->isTransitioning()) return;

	adjustedBrightness += brightnessStep;

	if (abs(targetBrightness - adjustedBrightness) < abs(brightnessStep)) {
		adjustedBrightness = targetBrightness;
	}

	FastLED.setBrightness(adjustedBrightness);
}

// -------------------- Primary Manager Loop -------------------

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::finishProgramForSection(LightSection<PIXEL, MAX_LIGHT_PROGRAMS, MAX_LAYERS> &section, uint32_t timeDelta) {
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
		section.outputBuffer->clear();
		section.transitionState = FINISHED;
		break;

		case FREEZE_FADE:
		// TODO: Implement

		case FADE_DOWN:
		if (!this->isTransitioning()) {
			section.outputBuffer->clear();
			section.transitionState = FINISHED;
			this->setBrightness(this->brightness);
		} else {
			section.activeProgram->update(timeDelta);
		}
		break;
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::loop() {
	uint32_t time = millis(), timeDelta = time - lastTime;
	lastTime = time;

	// TODO: Frame rate limiting isn't working quite right
	if (timeDelta < msPerFrame) {
		delay(msPerFrame - timeDelta);
		
		// return;
	}

	this->transitionBrightness();

	for (int i = 0; i < sectionCount; i++) {
		TLightProgram<PIXEL> *program = lightSections[i].activeProgram;
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
