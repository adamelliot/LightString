// -------------------------- Light Layer --------------------------

LIGHT_LAYER_TEMPLATE
ILightPattern *LIGHT_LAYER_CLASS::getPattern(PatternCode patternCode) {
	int copy = 0;
	for (int i = 0; i < patternCount; i++) {
		if (lightPatterns[i]->getPatternID() == patternCode.patternID) {
			if (copy == patternCode.copyID) {
				return lightPatterns[i];
			} else {
				copy++;
			}
		}
	}

	return NULL;
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::updatePatternIndex(PatternCode patternCode) {
	if (patternList[patternIndex] == patternCode) return;

	for (int i = 0; i < patternListLength; i++) {
		if (patternList[i] == patternCode) {
			patternIndex = i;
			break;
		}
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::finishPattern() {
	if (!activePattern) return;

	setPlayState(PROGRAM_FINISHED);
	activePattern->patternFinished();

	if (!activePattern->isFilterPattern()) {
		section->unlockBuffer(activePattern->getPixelBuffer());
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::setPlayState(EPlayState playState) {
	this->playState = playState;

	if (patternEventHandler) {
		patternEventHandler(*activePattern, playState);
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::play() {
	if (playState == PROGRAM_PLAYING) return;
	if (playState == PROGRAM_PAUSED) {
		unpause();
		return;
	}

	startPattern(patternList[patternIndex]);
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::stop() {
	if (playState == PROGRAM_STOPPED) return;
	finishPattern();

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
	patternStartedAt += timeDelta;
	transitionStartedAt += timeDelta;
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::startPattern(PatternCode patternCode) {
#ifdef VERBOSE
	Serial.print(F("Starting Pattern: 0x"));
	Serial.print(patternCode.patternID, HEX);
	Serial.print(F("\tMode: "));
	if (patternCode.mode == ANY_MODE) {
		Serial.print(F("R"));
	} else {
		Serial.print(patternCode.mode);
	}

	if (patternCode.copyID > 0) {
		Serial.print(F("\tC: "));
		Serial.print(patternCode.copyID, DEC);
	}
	Serial.print(F("\tLayer: "));
	Serial.println(layerID);
#endif

	bool randomMode = patternCode.mode == 0xff;
	if (randomMode) patternCode.mode = 0;

	ILightPattern *pattern = getPattern(patternCode);
	if (!pattern) {
#ifdef __DISPLAY_ERROR
		Serial.print("Pattern not found: 0x");
		Serial.print(patternCode.patternID, HEX);
		Serial.print(" on Layer: ");
		Serial.println(layerID);
#endif
		return false;
	}

	if (randomMode) {
		patternCode.mode = random(pattern->getModeCount());
	}

	finishPattern();
	updatePatternIndex(patternCode);

	this->opacity = 255;

	this->activePattern = pattern;
	this->activePattern->setLayer(this);

	if (this->activePattern->isFilterPattern()) {
		this->activePattern->setPixelBuffer(this->section->getOutputBuffer());
	} else {
		this->activePattern->setPixelBuffer(this->section->lockBuffer());
	}

	setPlayState(PROGRAM_STARTED);

	this->activePattern->setMode(patternCode.mode);
	this->patternStartedAt = millis();
	this->transitionState = TRANSITION_STARTING;

	runningBeginTransition = true;

	setPlayState(PROGRAM_PLAYING);

	return true;
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::startRandomPattern() {
	patternIndex = random(patternListLength);
	return nextPattern();
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::nextPattern() {
	patternIndex++;
	patternIndex %= patternListLength;

	return startPattern(patternList[patternIndex]);
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::prevPattern() {
	if (patternIndex == 0) {
		patternIndex = patternListLength - 1;
	} else {
		patternIndex--;
	}

	return startPattern(patternList[patternIndex]);
}

// Shuffle from: http://benpfaff.org/writings/clc/shuffle.html
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::shufflePatterns() {
	for (size_t i = 0; i < patternListLength; i++) {
		size_t j = (i + random() / (0xffffffff / (patternListLength - 1) + 1)) % patternListLength;
		PatternCode t = patternList[j];
		patternList[j] = patternList[i];
		patternList[i] = t;
	}
}

// TODO: This will only support up to 64 pattern modes currently, should really support 256
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::addLightPattern(ILightPattern &pattern, uint64_t modeList) {
	uint8_t patternID = pattern.getPatternID();

	int copyID = 0;
	for (int i = 0; i < patternCount; i++) {
		if (lightPatterns[i]->getPatternID() == patternID) {
			copyID++;
		}
	}

	lightPatterns[patternCount] = &pattern;
	patternCount++;

	if (!pattern.hideFromPatternList()) {
		for (uint8_t mode = 0; modeList; mode++) {
			if ((modeList & 1) == 0) {
				modeList >>= 1;
				continue;
			}
			modeList >>= 1;

#ifdef VERBOSE
			Serial.print(F("Adding Pattern Code: "));
			Serial.print((patternID << 8) | mode, HEX);
			Serial.print(" at: ");
			Serial.println((uint32_t)&pattern);
#endif

			patternList[patternListLength] = PatternCode(patternID, copyID, mode);
			patternListLength++;
		}
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::addLightPattern(ILightPattern &pattern) {
	uint64_t modeList = 0;

	for (int i = 0; i < pattern.getModeCount(); i++) {
		modeList |= 1 << i;
	}

	addLightPattern(pattern, modeList);
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

	EPatternTransition transition = runningBeginTransition ?
		activePattern->getBeginTransition() : activePattern->getEndTransition();
	
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
		activePattern->update(timeDelta);
		opacity = 255 - ratio;
		clear = true;
		break;

		case TRANSITION_FADE_UP:
		activePattern->update(timeDelta);
		opacity = ratio;
		break;
	}

	if (timeElapsed >= transitionLength) {
		transitionState = TRANSITION_DONE;
		if (clear) {
			activePattern->getPixelBuffer()->clear();
		}
	}

	if (transitionState == TRANSITION_DONE) {
		if (!runningBeginTransition) {
			switch (playMode) {
				case PLAY_MODE_CONTINUOUS:
				{
					PatternCode code = activePattern->getNextPatternCode();
					if (code == PatternCode(0xff, 0xff, 0xff)) {
						nextPattern();
					} else {
						startPattern(code);
					}
				}
				break;

				case PLAY_MODE_ONCE: // Once we're done
				finishPattern();
				setPlayState(PROGRAM_STOPPED);
				break;
			
				case PLAY_MODE_REPEAT:
				startPattern(patternList[patternIndex]);
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

	if (!activePattern || playState == PROGRAM_STOPPED || playState == PROGRAM_PAUSED) return;

	uint32_t patternTimeDelta = time - patternStartedAt;

	// NOTE: Should transition time adjust end time?
	if (activePattern->isPatternFinished() || 
		(activePattern->getPatternLength() > 0 && patternTimeDelta > activePattern->getPatternLength()) ||
		(maxPatternLength > 0 && patternTimeDelta > maxPatternLength))
	{
		if (transitionState == TRANSITION_DONE) {
			transitionState = TRANSITION_STARTING;
		}
	}

	if (transitionState != TRANSITION_DONE) {
		updateTransition(timeDelta);
	} else {
		activePattern->update(timeDelta);
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
#ifdef ARDUINO
		Serial.println(F("ERROR: Buffer added to pool needs to be the same size as the output buffer."));
#else 
		frprintf(stderr, "ERROR: Buffer added to pool needs to be the same size as the output buffer.");
#endif
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
/*
	for (int i = 0; i < MAX_LAYERS; i++) {
		layers[i].update();
		ILightPattern *pattern = layers[i].getActivePattern();
		
		if (pattern && !pattern->isFilterPattern() && bufferCount > 0) {
			TPixelBuffer<RGBA> *buffer = (TPixelBuffer<RGBA> *)pattern->getPixelBuffer();
			// TODO: Fix this so it's not destructive
			if (layers[i].getOpacity() < 255) {
				for (uint16_t j = 0; j < buffer->getLength(); j++) {
					buffer->pixels[j].a = scale8(buffer->pixels[j].a, layers[i].getOpacity());
				}
			}
			
			// Serial.print("Blend: ");
			// Serial.println(pattern->getBlendMode());
			outputBuffer->applyBlend(*buffer, pattern->getBlendMode());
		}
	}
	*/
}

// ------------------------ Pattern Manager ------------------------

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

// ------------------------ Pattern Management ------------------------

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::setPatternEventHandler(PatternEvent patternEventHandler) {
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].setPatternEventHandler(patternEventHandler);
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::setPatternEventHandler(PatternEvent patternEventHandler, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].setPatternEventHandler(patternEventHandler);
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::setMaxPatternLength(uint32_t maxPatternLength) {
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].setMaxPatternLength(maxPatternLength);
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::setMaxPatternLength(uint32_t maxPatternLength, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].setMaxPatternLength(maxPatternLength);
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
bool PROGRAM_MANAGER_CLASS::startPattern(PatternCode patternCode, uint8_t layerID, uint8_t sectionID) {
	return sections[sectionID].layers[layerID].startPattern(patternCode);
}

PROGRAM_MANAGER_TEMPLATE
bool PROGRAM_MANAGER_CLASS::startPattern(uint8_t patternID, uint8_t layerID) {
	return startPattern(PatternCode(patternID), 0, 0);
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::startRandomPattern() {
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			if (sections[i].layers[j].isActive()) {
				sections[i].layers[j].startRandomPattern();
			}
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::startRandomPatternOnAllLayers() {
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].startRandomPattern();
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::startRandomPattern(uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].startRandomPattern();
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
void PROGRAM_MANAGER_CLASS::nextPattern(uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].nextPattern();
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::nextPattern() {
	for (int j = 0; j < sectionCount; j++) {
		for (int i = 0; i < MAX_LAYERS; i++) {
			sections[j].layers[i].nextPattern();
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::prevPattern(uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].prevPattern();
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::prevPattern() {
	for (int j = 0; j < sectionCount; j++) {
		for (int i = 0; i < MAX_LAYERS; i++) {
			sections[j].layers[i].prevPattern();
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::shufflePatterns() {
	for (int i = 0; i < sectionCount; i++) {
		for (int j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].shufflePatterns();
		}
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::addLightPattern(ILightPattern &pattern, uint8_t layerID) {
	for (int i = 0; i < sectionCount; i++) {
		sections[i].layers[layerID].addLightPattern(pattern);
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::addLightPattern(ILightPattern &pattern, uint64_t modeList, uint8_t layerID) {
	for (int i = 0; i < sectionCount; i++) {
		sections[i].layers[layerID].addLightPattern(pattern, modeList);
	}
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::addLightPattern(ILightPattern &pattern, uint64_t modeList, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].addLightPattern(pattern, modeList);
}

// -------------------- Adding Sections -------------------

PROGRAM_MANAGER_TEMPLATE
LIGHT_SECTION_CLASS *PROGRAM_MANAGER_CLASS::getLightSection(uint8_t sectionID) {
	if (sectionID >= sectionCount) return NULL;

	return &sections[sectionID];
}

PROGRAM_MANAGER_TEMPLATE
uint8_t PROGRAM_MANAGER_CLASS::addLightSection(TPixelBuffer<PIXEL, FORMAT> &pixelBuffer) {
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
#ifdef USE_FASTLED
		FastLED.setBrightness(adjustedBrightness);
#endif
	}

	targetBrightness = brightness;
	brightnessStep = (brightness - adjustedBrightness) / kTransitionFrames;
}

PROGRAM_MANAGER_TEMPLATE
void PROGRAM_MANAGER_CLASS::setBrightness(uint8_t brightness) {
	this->brightness = brightness;
	this->adjustedBrightness = brightness;
	this->targetBrightness = brightness;

#ifdef USE_FASTLED
	FastLED.setBrightness(brightness);
#endif
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

#ifdef USE_FASTLED
	FastLED.setBrightness(adjustedBrightness);
#endif
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
