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
#ifdef ARDUINO
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
#else
	printf("Starting Pattern: 0x%x\tMode: ", patternCode.patternID);
	if (patternCode.mode == ANY_MODE) {
		printf("R");
	} else {
		printf("%d", patternCode.mode);
	}
	if (patternCode.copyID > 0) {
		printf("\tC: %d", patternCode.copyID);
	}
	printf("\tLayer: %d\n", layerID);
#endif
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
#ifdef ARDUINO
			Serial.print(F("Adding Pattern Code: "));
			Serial.print((patternID << 8) | mode, HEX);
			Serial.print(" at: ");
			Serial.println((uint32_t)&pattern);
#else
			printf("Adding Pattern Code: 0x%x\n", (patternID << 8) | mode);
#endif
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
