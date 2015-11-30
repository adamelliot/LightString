// -------------------------- Light Layer --------------------------

LIGHT_LAYER_TEMPLATE
ILightProgram *LIGHT_LAYER_CLASS::getProgram(ProgramCode &programCode) {
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

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::updateProgramIndex(ProgramCode &programCode) {
	if (programList[programIndex] == programCode) return;

	for (int i = 0; i < programListLength; i++) {
		if (programList[i] == programCode) {
			programIndex = i;
			break;
		}
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::pause() {
	if (playState == PROGRAM_PAUSED || playState == PROGRAM_STOPPED) return;

	playState = PROGRAM_PAUSED;
	pauseStartedAt = millis();
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::unpause() {
	if (playState != PROGRAM_PAUSED) return;

	playState = PROGRAM_PLAYING;
	
	uint32_t timeDelta = millis() - pauseStartedAt;
	programStartedAt += timeDelta;
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::startProgram(ProgramCode &programCode) {
#ifdef VERBOSE
	Serial.println(sizeof(*this));
	Serial.print(F("Selecting Program: "));
	Serial.print((programCode.programID << 8) | programCode.mode, HEX);
	Serial.print(F(" ("));
	Serial.print(programCode.copyID, DEC);
	Serial.print(F(") Layer: "));
	Serial.println(layerID);
#endif

	ILightProgram *program = getProgram(programCode);
	if (!program) {
		return false;
	}

	updateProgramIndex(programCode);

	playState = PROGRAM_FINISHED;
	
	if (programEventHandler) {
		// TODO: Implement callbacks
		// programEventHandler(*section.activeProgram, playState);
	}

	if (this->activeProgram && !this->activeProgram->isFilterProgram()) {
		this->section->unlockBuffer(this->activeProgram->getPixelBuffer());
	}

	this->activeProgram = program;
	
	if (this->activeProgram->isFilterProgram()) {
		this->activeProgram->setPixelBuffer(this->section->outputBuffer);
	} else {
		this->activeProgram->setPixelBuffer(this->section->lockBuffer());
	}

	playState = PROGRAM_STARTED;

	if (programEventHandler) {
		// TODO: Implement callbacks
		// programEventHandler(*section.activeProgram, playState);
	}

	this->activeProgram->setMode(programCode.mode);
	this->programStartedAt = millis();

	playState = PROGRAM_PLAYING;

	return true;
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::startRandomProgram() {
	programIndex = random(programListLength);
	return nextProgram();
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::nextProgram() {
	programIndex++;
	programIndex %= programListLength;

	return startProgram(programList[programIndex]);
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::prevProgram() {
	if (programIndex == 0) {
		programIndex = programListLength - 1;
	} else {
		programIndex--;
	}

	return startProgram(programList[programIndex]);
}

// Shuffle from: http://benpfaff.org/writings/clc/shuffle.html
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::randomizeProgramOrder() {
	for (size_t i = 0; i < programListLength; i++) {
		size_t j = i + random() / (0xffff / (programListLength - 1) + 1);
		ProgramCode t = programList[j];
		programList[j] = programList[i];
		programList[i] = t;
	}
}

// TODO: This will only support up to 64 progrom modes currently, should really support 256
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::addLightProgram(ILightProgram &program, uint64_t modeList) {
	uint8_t programID = program.getProgramID();

	int copyID = 0;
	for (int i = 0; i < programCount; i++) {
		if (lightPrograms[i]->getProgramID() == programID) {
			copyID++;
		}
	}

	lightPrograms[programCount] = &program;
	programCount++;

	if (!program.hideFromProgramList()) {
		for (uint8_t mode = 0; modeList; mode++) {
			if ((modeList & 1) == 0) {
				modeList >>= 1;
				continue;
			}
			modeList >>= 1;

#ifdef VERBOSE
			Serial.print(F("Adding Program Code: "));
			Serial.print((programID << 8) | mode, HEX);
			Serial.print(" at: ");
			Serial.println((uint32_t)&program);
#endif

			programList[programListLength] = ProgramCode(programID, copyID, mode);
			programListLength++;
		}
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::addLightProgram(ILightProgram &program) {
	uint64_t modeList = 0;

	for (int i = 0; i < program.getModeCount(); i++) {
		modeList |= 1 << i;
	}

	addLightProgram(program, modeList);
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::updateTranstion(uint32_t timeDelta) {
	if (transitionState == TRANSITION_STARTING) {
		transitionState = TRANSITION_RUNNING;
	}

	switch (activeProgram->getTransition()) {
		case OVERWRITE:
		transitionState = TRANSITION_DONE;
		break;

		case WIPE:
		activeProgram->getPixelBuffer()->clear();
		transitionState = TRANSITION_DONE;
		break;

		case FREEZE_FADE:
		// TODO: Implement

		case FADE_DOWN:
		// TODO: Implement
		transitionState = TRANSITION_DONE;
		break;
	}

	if (transitionState == TRANSITION_DONE) {
		nextProgram();
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::update() {
	uint32_t time = millis(), timeDelta = time - lastTime;
	lastTime = time;

	uint32_t programTimeDelta = time - programStartedAt;

	// NOTE: Should transition time adjust end time?
	if (activeProgram->isProgramFinished() || 
		(activeProgram->getProgramLength() > 0 && programTimeDelta > activeProgram->getProgramLength()) ||
		(maxProgramLength > 0 && programTimeDelta > maxProgramLength))
	{
		if (transitionState == TRANSITION_DONE) {
			transitionState = TRANSITION_STARTING;
		}
	}

	if (transitionState != TRANSITION_DONE) {
		updateTranstion(timeDelta);
	} else {
		activeProgram->update(timeDelta);
	}
}

// ------------------------ Program Manager ------------------------

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::pause(bool blackout, bool fade) {
/*	if (paused) return;

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
	}*/
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::unpause() {
/*	if (!paused) return;

	this->fadeUp();

	paused = false;
	uint32_t timeDelta = millis() - pauseStartedAt;
	for (int i = 0; i < sectionCount; i++) {
		lightSections[i].programStartedAt += timeDelta;
	}*/
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::togglePause() {
//	paused ? unpause() : pause();
}

/*
PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::nudge(int32_t data) {
	for (int i = 0; i < sectionCount; i++) {
		lightSections[i].activeProgram->nudge(data);
	}
}*/


// ------------------------ Program Management ------------------------

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::setMaxProgramLength(uint32_t maxProgramLength) {
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].setMaxProgramLength(maxProgramLength);
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::setMaxProgramLength(uint32_t maxProgramLength, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].setMaxProgramLength(maxProgramLength);
}

PROGRAM_MANAGER_TEMPLATE
bool PROGRAM_MANAGER_CLASS::startProgram(ProgramCode &programCode, uint8_t sectionID, uint8_t layerID) {
	return sections[sectionID].layers[layerID].startProgram(programCode);
}

PROGRAM_MANAGER_TEMPLATE
bool PROGRAM_MANAGER_CLASS::startProgram(uint8_t programID, uint8_t layerID) {
	return startProgram(ProgramCode(programID), 0, 0);
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::startRandomProgram(bool activateLayers) {
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			if (sections[i].layers[j].isActive() || activateLayers) {
				sections[i].layers[j].startRandomProgram();
			}
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::startRandomProgram(uint8_t sectionID, uint8_t layerID) {
	sections[sectionID].layers[layerID].startRandomProgram();
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::nextProgram(uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].nextProgram();
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::nextProgram() {
	for (int j = 0; j < sectionCount; j++) {
		for (int i = 0; i < MAX_LAYERS; i++) {
			sections[j].layers[i].nextProgram();
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::prevProgram(uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].prevProgram();
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::prevProgram() {
	for (int j = 0; j < sectionCount; j++) {
		for (int i = 0; i < MAX_LAYERS; i++) {
			sections[j].layers[i].prevProgram();
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::randomizeProgramOrder() {
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].randomizeProgramOrder();
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::addLightProgram(ILightProgram &program, uint8_t layerID) {
	for (int i = 0; i < sectionCount; i++) {
		sections[i].layers[layerID].addLightProgram(program);
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::addLightProgram(ILightProgram &program, uint64_t modeList, uint8_t layerID) {
	for (int i = 0; i < sectionCount; i++) {
		sections[i].layers[layerID].addLightProgram(program, modeList);
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::addLightProgram(ILightProgram &program, uint64_t modeList, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].addLightProgram(program, modeList);
}

// -------------------- Adding Sections -------------------

PROGRAM_MANAGER_TEMPLATE
LIGHT_SECTION_CLASS *PROGRAM_MANAGER_CLASS::getLightSection(uint8_t sectionID) {
	if (sectionID >= sectionCount) return NULL;

	return &sections[sectionID];
}

PROGRAM_MANAGER_TEMPLATE
uint8_t PROGRAM_MANAGER_CLASS::addLightSection(CRGBBuffer &pixelBuffer) {
	if (sectionCount >= MAX_LIGHT_SECTIONS) {
#ifdef VERBOSE
		Serial.println(F("ERROR: Maximum amount of light sections already added."));
#endif
		return 0xff;
	}

	sections[sectionCount].outputBuffer = &pixelBuffer;

	return sectionCount++;
}

PROGRAM_MANAGER_TEMPLATE
bool PROGRAM_MANAGER_CLASS::addBufferToLightSection(uint8_t sectionID, IPixelBuffer &buffer) {
	LIGHT_SECTION_CLASS *lightSection = getLightSection(sectionID);
	if (!lightSection) return false;

	return lightSection->addBuffer(&buffer);
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
		sections[i].update();
	}
}
