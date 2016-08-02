LIGHT_LAYER_TEMPLATE
ILightPattern *LIGHT_LAYER_CLASS::getPattern(PatternCode patternCode) {
	int copy = 0;
	for (uint32_t i = 0; i < lightPatterns.size(); i++) {
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

	for (uint32_t i = 0; i < patternList.size(); i++) {
		if (patternList[i] == patternCode) {
			patternIndex = i;
			break;
		}
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::finishPattern() {
	if (!activePattern) return;

	setPlayState(PATTERN_FINISHED);
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
	if (playState == PATTERN_PLAYING) return;
	if (playState == PATTERN_PAUSED) {
		unpause();
		return;
	}

	startPattern(patternList[patternIndex]);
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::stop() {
	if (playState == PATTERN_STOPPED) return;
	finishPattern();

	setPlayState(PATTERN_STOPPED);
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::pause() {
	if (playState == PATTERN_PAUSED || playState == PATTERN_STOPPED) return;

	setPlayState(PATTERN_PAUSED);

	pauseStartedAt = millis();
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::unpause() {
	if (playState != PATTERN_PAUSED) return;

	setPlayState(PATTERN_PLAYING);
	
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

	this->opacity = getMaxOpacity();

	this->activePattern = pattern;
	this->activePattern->setLayer(this);

	if (this->activePattern->isFilterPattern()) {
		this->activePattern->setPixelBuffer(this->section->getOutputBuffer());
	} else {
		this->activePattern->setPixelBuffer(this->section->lockBuffer());
	}

	setPlayState(PATTERN_STARTED);

	this->activePattern->setMode(patternCode.mode);
	this->patternStartedAt = millis();
	this->transitionState = TRANSITION_STARTING;

	runningBeginTransition = true;

	setPlayState(PATTERN_PLAYING);

	return true;
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::startRandomPattern() {
	patternIndex = random(patternList.size());
	return nextPattern();
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::nextPattern() {
	patternIndex++;
	patternIndex %= patternList.size();

	return startPattern(patternList[patternIndex]);
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::prevPattern() {
	if (patternIndex == 0) {
		patternIndex = patternList.size() - 1;
	} else {
		patternIndex--;
	}

	return startPattern(patternList[patternIndex]);
}

// Shuffle from: http://benpfaff.org/writings/clc/shuffle.html
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::shufflePatterns() {
	for (size_t i = 0; i < patternList.size(); i++) {
		size_t j = (i + random() / (0xfffffff / (patternList.size() - i) + 1)) % patternList.size();
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
	for (int i = 0; i < lightPatterns.size(); i++) {
		if (lightPatterns[i]->getPatternID() == patternID) {
			copyID++;
		}
	}

	lightPatterns.push_back(&pattern);

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

			patternList.push_back(PatternCode(patternID, copyID, mode));
		}
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::addLightPattern(ILightPattern &pattern) {
	uint64_t modeList = 0;

	for (uint32_t i = 0; i < pattern.getModeCount(); i++) {
		modeList |= 1 << i;
	}

	addLightPattern(pattern, modeList);
}

template <>
inline float LightLayer<float>::getElapsedTimeRatio() {
	uint32_t timeElapsed = millis() - transitionStartedAt;
	float ratio = timeElapsed / transitionLength;
	if (ratio > 1.0f) ratio = 1.0f;

	return ratio;
}

template <>
inline uint8_t LightLayer<uint8_t>::getElapsedTimeRatio() {
	uint32_t timeElapsed = millis() - transitionStartedAt;
	uint32_t ratio = timeElapsed * 256 / transitionLength;
	if (ratio > 255) ratio = 255;
	return (uint8_t)ratio;
}

template <>
inline uint8_t LightLayer<uint8_t>::getMaxOpacity() { return 0xff; }

template <>
inline float LightLayer<float>::getMaxOpacity() { return 1.0f; }

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::updateTransition(uint32_t timeDelta) {
	if (transitionState == TRANSITION_STARTING) {
		transitionState = TRANSITION_RUNNING;
		transitionStartedAt = millis();
	}

	uint32_t timeElapsed = millis() - transitionStartedAt;
	FORMAT ratio = getElapsedTimeRatio();
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
		opacity = getMaxOpacity() - ratio;
		clear = true;
		break;

		case TRANSITION_FADE_DOWN:
		activePattern->update(timeDelta);
		opacity = getMaxOpacity() - ratio;
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
				setPlayState(PATTERN_STOPPED);
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

	if (!activePattern || playState == PATTERN_STOPPED || playState == PATTERN_PAUSED) return;

	uint32_t patternTimeDelta = time - patternStartedAt;

	// NOTE: Should transition time adjust end time?
	if (activePattern->isPatternFinished() || 
		(activePattern->getPatternLength() > 0 && patternTimeDelta > (uint32_t)activePattern->getPatternLength()) ||
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
