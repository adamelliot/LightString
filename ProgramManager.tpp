// -------------------------- Light Layer --------------------------

LIGHT_LAYER_TEMPLATE
ILightProgram *LIGHT_LAYER_CLASS::getProgram(ProgramCode programCode) {
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
void LIGHT_LAYER_CLASS::updateProgramIndex(ProgramCode programCode) {
	if (programList[programIndex] == programCode) return;

	for (int i = 0; i < programListLength; i++) {
		if (programList[i] == programCode) {
			programIndex = i;
			break;
		}
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::finishProgram() {
	if (!activeProgram) return;

	setPlayState(PROGRAM_FINISHED);
	activeProgram->programFinished();

	if (!activeProgram->isFilterProgram()) {
		section->unlockBuffer(activeProgram->getPixelBuffer());
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::setPlayState(EPlayState playState) {
	this->playState = playState;

	if (programEventHandler) {
		programEventHandler(*activeProgram, playState);
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::play() {
	if (playState == PROGRAM_PLAYING) return;
	if (playState == PROGRAM_PAUSED) {
		unpause();
		return;
	}

	startProgram(programList[programIndex]);
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::stop() {
	if (playState == PROGRAM_STOPPED) return;
	finishProgram();

	setPlayState(PROGRAM_STOPPED);
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::pause() {
	if (playState == PROGRAM_PAUSED || playState == PROGRAM_STOPPED) return;

	setPlayState(PROGRAM_PAUSED);

	pauseStartedAt = millis();
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::unpause() {
	if (playState != PROGRAM_PAUSED) return;

	setPlayState(PROGRAM_PLAYING);
	
	uint32_t timeDelta = millis() - pauseStartedAt;
	programStartedAt += timeDelta;
	transitionStartedAt += timeDelta;
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::startProgram(ProgramCode programCode) {
#ifdef VERBOSE
	Serial.print(F("Starting Program: 0x"));
	Serial.print(programCode.programID, HEX);
	Serial.print(F("\tMode: "));
	if (programCode.mode == ANY_MODE) {
		Serial.print(F("R"));
	} else {
		Serial.print(programCode.mode);
	}

	if (programCode.copyID > 0) {
		Serial.print(F("\tC: "));
		Serial.print(programCode.copyID, DEC);
	}
	Serial.print(F("\tLayer: "));
	Serial.println(layerID);
#endif

	bool randomMode = programCode.mode == 0xff;
	if (randomMode) programCode.mode = 0;

	ILightProgram *program = getProgram(programCode);
	if (!program) {
		Serial.print("Program not found: 0x");
		Serial.print(programCode.programID, HEX);
		Serial.print(" on Layer: ");
		Serial.println(layerID);
		return false;
	}

	if (randomMode) {
		programCode.mode = random8(program->getModeCount());
	}

	finishProgram();
	updateProgramIndex(programCode);

	this->opacity = 255;

	this->activeProgram = program;
	this->activeProgram->setLayer(this);

	if (this->activeProgram->isFilterProgram()) {
		this->activeProgram->setPixelBuffer(this->section->getOutputBuffer());
	} else {
		this->activeProgram->setPixelBuffer(this->section->lockBuffer());
	}

	setPlayState(PROGRAM_STARTED);

	this->activeProgram->setMode(programCode.mode);
	this->programStartedAt = millis();
	this->transitionState = TRANSITION_STARTING;

	runningBeginTransition = true;

	setPlayState(PROGRAM_PLAYING);

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
void LIGHT_LAYER_CLASS::shufflePrograms() {
	for (size_t i = 0; i < programListLength; i++) {
		size_t j = (i + random() / (0xffffffff / (programListLength - 1) + 1)) % programListLength;
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
void LIGHT_LAYER_CLASS::updateTransition(uint32_t timeDelta) {
	if (transitionState == TRANSITION_STARTING) {
		transitionState = TRANSITION_RUNNING;
		transitionStartedAt = millis();
	}

	uint32_t timeElapsed = millis() - transitionStartedAt;
	uint32_t ratio = timeElapsed * 256 / transitionLength;
	if (ratio > 255) ratio = 255;
	
	bool clear = false;

	EProgramTransition transition = runningBeginTransition ?
		activeProgram->getBeginTransition() : activeProgram->getEndTransition();
	
	switch (transition) {
		case TRANSITION_OVERWRITE:
		timeElapsed = transitionLength;
		break;

		case TRANSITION_WIPE:
		timeElapsed = transitionLength;
		clear = true;
		break;

		case TRANSITION_FREEZE_FADE:
		opacity = 255 - ratio;
		clear = true;
		break;

		case TRANSITION_FADE_DOWN:
		activeProgram->update(timeDelta);
		opacity = 255 - ratio;
		clear = true;
		break;

		case TRANSITION_FADE_UP:
		activeProgram->update(timeDelta);
		opacity = ratio;
		break;
	}

	if (timeElapsed >= transitionLength) {
		transitionState = TRANSITION_DONE;
		if (clear) {
			activeProgram->getPixelBuffer()->clear();
		}
	}

	if (transitionState == TRANSITION_DONE) {
		if (!runningBeginTransition) {
			switch (playMode) {
				case PLAY_MODE_CONTINUOUS:
				{
					ProgramCode code = activeProgram->getNextProgramCode();
					if (code == ProgramCode(0xff, 0xff, 0xff)) {
						nextProgram();
					} else {
						startProgram(code);
					}
				}
				break;

				case PLAY_MODE_ONCE: // Once we're done
				finishProgram();
				setPlayState(PROGRAM_STOPPED);
				break;
			
				case PLAY_MODE_REPEAT:
				startProgram(programList[programIndex]);
				break;
			}
		} else {
			runningBeginTransition = false;
		}
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::update() {
	uint32_t time = millis(), timeDelta = time - lastTime;
	lastTime = time;

	if (!activeProgram || playState == PROGRAM_STOPPED || playState == PROGRAM_PAUSED) return;

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
		updateTransition(timeDelta);
	} else {
		activeProgram->update(timeDelta);
	}
}

// ------------------------- Light Section -------------------------

LIGHT_SECTION_TEMPLATE
IPixelBuffer *LIGHT_SECTION_CLASS::lockBuffer() {
	// In the case of no buffer pool the output buffer is used.
	// This only works if PIXEL is CRGB, and should otherwise throw an error
	// outputBuffer
	if (bufferCount == 0) return (TPixelBuffer<PIXEL> *)outputBuffer;
	
	for (int i = 0; i < bufferCount; i++) {
		uint8_t bit = 1 << i;
		if ((activeBuffers & bit) == 0) {
			activeBuffers |= bit;
			return bufferPool[i];
		}
	}

	return NULL;
}

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::unlockBuffer(IPixelBuffer *buffer) {
	if (buffer == (IPixelBuffer *)outputBuffer) return;

	for (int i = 0; i < bufferCount; i++) {
		if (bufferPool[i] == buffer) {
			uint8_t bit = 1 << i;
			activeBuffers &= ~bit;
		}
	}
}

LIGHT_SECTION_TEMPLATE
bool LIGHT_SECTION_CLASS::addBuffer(IPixelBuffer *buffer) {
	if (!buffer->getLength() == this->outputBuffer->getLength()) {
#ifdef VERBOSE
		Serial.println(F("ERROR: Buffer added to pool needs to be the same size as the output buffer."));
#endif

		return false;
	}

	if (bufferCount >= (MAX_LAYERS * 2)) return false;
	bufferPool[bufferCount++] = buffer;

	return true;
}

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::update() {
	if (bufferCount > 0) {
		outputBuffer->clear();
	}

	for (int i = 0; i < MAX_LAYERS; i++) {
		layers[i].update();
		ILightProgram *program = layers[i].getActiveProgram();
		
		if (program && !program->isFilterProgram() && bufferCount > 0) {
			TPixelBuffer<RGBA> *buffer = (TPixelBuffer<RGBA> *)program->getPixelBuffer();
			// TODO: Fix this so it's not destructive
			if (layers[i].getOpacity() < 255) {
				for (uint16_t j = 0; j < buffer->getLength(); j++) {
					buffer->pixels[j].a = scale8(buffer->pixels[j].a, layers[i].getOpacity());
				}
			}
			
			// Serial.print("Blend: ");
			// Serial.println(program->getBlendMode());
			outputBuffer->applyBlend(*buffer, program->getBlendMode());
		}
	}
}

// ------------------------ Program Manager ------------------------

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::pause(bool blackout, bool fade) {
	if (blackout) {
		if (fade) {
			fadeDown();
		} else {
			setBrightness(0);
		}
	}

	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].pause();
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::unpause() {
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].unpause();
		}
	}
}

// ------------------------ Program Management ------------------------

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::setProgramEventHandler(ProgramEvent programEventHandler) {
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].setProgramEventHandler(programEventHandler);
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::setProgramEventHandler(ProgramEvent programEventHandler, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].setProgramEventHandler(programEventHandler);
}

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
void PROGRAM_MANAGER_CLASS::setPlayMode(EPlayMode playMode) {
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].setPlayMode(playMode);
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::setPlayMode(EPlayMode playMode, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].setPlayMode(playMode);
}

PROGRAM_MANAGER_TEMPLATE
bool PROGRAM_MANAGER_CLASS::startProgram(ProgramCode programCode, uint8_t layerID, uint8_t sectionID) {
	return sections[sectionID].layers[layerID].startProgram(programCode);
}

PROGRAM_MANAGER_TEMPLATE
bool PROGRAM_MANAGER_CLASS::startProgram(uint8_t programID, uint8_t layerID) {
	return startProgram(ProgramCode(programID), 0, 0);
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::startRandomProgram() {
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			if (sections[i].layers[j].isActive()) {
				sections[i].layers[j].startRandomProgram();
			}
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::startRandomProgramOnAllLayers() {
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].startRandomProgram();
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::startRandomProgram(uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].startRandomProgram();
}


PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::play() {
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].play();
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::play(uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].play();
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
void PROGRAM_MANAGER_CLASS::shufflePrograms() {
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].shufflePrograms();
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
uint8_t PROGRAM_MANAGER_CLASS::addLightSection(TPixelBuffer<RGB> &pixelBuffer) {
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
bool PROGRAM_MANAGER_CLASS::loop() {
	uint32_t time = millis(), timeDelta = time - lastTime;

	// TODO: Frame rate limiting isn't working quite right
	if (timeDelta < msPerFrame) {
		// delay(msPerFrame - timeDelta);
		return false;
	}
	lastTime = time;

	this->transitionBrightness();

	for (int i = 0; i < sectionCount; i++) {
		sections[i].update();
	}
	
	return true;
}
