LIGHT_LAYER_TEMPLATE
LIGHT_LAYER_CLASS::~LightLayer() {
	delete patternSequence;
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::setPatternSequence(const PatternSequence &patternSequence) {
	this->patternSequence = new PatternSequence(patternSequence);
	if (patternIndex >= this->patternSequence->getSequence().size()) {
		patternIndex = 0;
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::setPatternEventHandler(PatternEvent patternEventHandler, void *userData) {
	this->config.patternEventHandler = patternEventHandler;
	this->config.patternEventUserData = userData;
}

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

	return nullptr;
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::updatePatternIndex(PatternCode patternCode) {

	if (patternSequence) {
		auto sequence = patternSequence->getSequence();
		auto size = sequence.size();
		uint32_t i = patternIndex;

		while (size--) {
			auto cue = patternSequence->getPatternCue(i);
			if (cue.code == patternCode) {
				patternIndex = i;
				break;
			}

			i++;
			i %= sequence.size();
		}
	} else {
		if (patternList[patternIndex] == patternCode) return;

		auto size = patternList.size();
		for (uint32_t i = 0; i < size; i++) {
			if (patternList[(i + patternIndex) % size] == patternCode) {
				patternIndex = i;
				break;
			}
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

	if (this->activePattern != nullptr && patternCloned) delete this->activePattern;
	patternCloned = false;
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::setPlayState(EPlayState playState) {
	this->playState = playState;

	if (config.patternEventHandler) {
		config.patternEventHandler(*activePattern, playState, config.eventHandlerUserData);
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::play() {
	if (playState == PATTERN_PLAYING) return;
	if (playState == PATTERN_PAUSED) {
		unpause();
		return;
	}

	startSelectedPattern();
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
void LIGHT_LAYER_CLASS::startPattern(ILightPattern *pattern, uint8_t mode, PatternConfig *config) {
	this->opacity = getMaxOpacity();

	if (clonePatterns) {
		auto clonedPattern = pattern->clone();
		if (!clonedPattern) {
			clonedPattern = pattern;
#ifdef __DISPLAY_ERROR
#ifdef ARDUINO
			Serial.print("Pattern not cloned, running original pattern -- pattern: 0x");
			Serial.print(pattern->getGatternID(), HEX);
			Serial.print(" on Layer: ");
			Serial.println(layerID);
#else
			printf("Pattern not cloned, running original pattern -- pattern: 0x%x\t on Layer: %d\n",
				pattern->getGatternID(), layerID);
#endif
#endif
		} else {
			patternCloned = true;
		}

		this->activePattern = clonedPattern;
	} else {
		this->activePattern = pattern;
	}

	this->activePattern->setLayer(this);

	if (this->activePattern->isFilterPattern()) {
		this->activePattern->setPixelBuffer(this->section->getOutputBuffer());
	} else {
		this->activePattern->setPixelBuffer(this->section->lockBuffer());
	}

	setPlayState(PATTERN_STARTED);

	if (config) {
		this->activePattern->setConfig(*config);
	}
	this->activePattern->setMode(mode);

	this->patternStartedAt = millis();
	this->transitionState = TRANSITION_STARTING;

	runningBeginTransition = true;

	setPlayState(PATTERN_PLAYING);
}

/**
 * Starts the pattern at patternIndex and chooses between sequence or patternList
 */
LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::startSelectedPattern() {
	finishPattern();

	PatternCode code;
	PatternConfig *config = nullptr;

	if (patternSequence) {
		auto cue = patternSequence->getPatternCue(patternIndex);
		code = cue.code;
		config = &cue;
	} else {
		code = patternList[patternIndex];
	}

	auto pattern = getPattern(code);

	if (!pattern) {
#ifdef __DISPLAY_ERROR
		Serial.print("Pattern not found: 0x");
		Serial.print(cue.code.patternID, HEX);
		Serial.print(" on Layer: ");
		Serial.println(layerID);
#endif
		return false;
	}

	startPattern(pattern, code.mode, config);
	return true;
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::startPattern(PatternCode patternCode) {
#ifdef LS_VERBOSE
#ifdef ARDUINO
	Serial.print(F("Starting Pattern: 0x"));
	Serial.print(patternCode.patternID, HEX);
	Serial.print(F("\tMode: "));
	if (patternCode.mode == ALL_MODES) {
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
	if (patternCode.mode == ALL_MODES) {
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

	startPattern(pattern, patternCode.mode);

	return true;
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::startRandomPattern() {
	uint32_t size = patternList.size();

	if (patternSequence) {
		size = patternSequence->getSequence().size();
	}

	patternIndex = random(size);

	return startSelectedPattern();
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::nextPattern() {
	uint32_t size = patternList.size();

	if (patternSequence) {
		size = patternSequence->getSequence().size();
	}

	patternIndex++;
	patternIndex %= size;

	return startSelectedPattern();
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::prevPattern() {
	uint32_t size = patternList.size();

	if (patternSequence) {
		size = patternSequence->getSequence().size();
	}
	if (patternIndex == 0) {
		patternIndex = size - 1;
	} else {
		patternIndex--;
	}

	return startSelectedPattern();
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

#ifdef LS_VERBOSE
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
	if (config.transitionDuration <= 0.0f) return 1.0f;

	uint32_t timeElapsed = millis() - transitionStartedAt;
	float ratio = (float)timeElapsed / (float)config.transitionDuration;
	if (ratio > 1.0f) ratio = 1.0f;

	return ratio;
}

template <>
inline uint8_t LightLayer<uint8_t>::getElapsedTimeRatio() {
	if (config.transitionDuration == 0) return 255;

	uint32_t timeElapsed = millis() - transitionStartedAt;
	uint32_t ratio = timeElapsed * 256 / config.transitionDuration;
	if (ratio > 255) ratio = 255;
	return (uint8_t)ratio;
}

template <>
inline uint8_t LightLayer<uint8_t>::getMaxOpacity() { return 0xff; }

template <>
inline float LightLayer<float>::getMaxOpacity() { return 1.0f; }

LIGHT_LAYER_TEMPLATE
inline EPatternTransition LIGHT_LAYER_CLASS::getSelectedInTransition() {
	if (activePattern->getInTransition() == TRANSITION_DEFAULT) {
		if (patternSequence) {
			auto cue = patternSequence->getPatternCue(patternIndex);
			if (cue.inTransition == TRANSITION_DEFAULT) {
				return config.inTransition;
			} else {
				return cue.inTransition;
			}
		} else {
			return config.inTransition;
		}
	}

	return activePattern->getInTransition();
}

LIGHT_LAYER_TEMPLATE
inline EPatternTransition LIGHT_LAYER_CLASS::getSelectedOutTransition() {
	if (activePattern->getOutTransition() == TRANSITION_DEFAULT) {
		if (patternSequence) {
			auto cue = patternSequence->getPatternCue(patternIndex);
			if (cue.outTransition == TRANSITION_DEFAULT) {
				return config.outTransition;
			} else {
				return cue.outTransition;
			}
		} else {
			return config.outTransition;
		}
	}

	return activePattern->getOutTransition();
}

LIGHT_LAYER_TEMPLATE
inline int32_t LIGHT_LAYER_CLASS::getSelectedPatternDuration() {
	if (activePattern->getPatternDuration() == -1) {
		if (patternSequence) {
			auto cue = patternSequence->getPatternCue(patternIndex);
			if (cue.patternDuration == -1) {
				return config.patternDuration;
			} else {
				return cue.patternDuration;
			}
		} else {
			return config.patternDuration;
		}
	}

	return activePattern->getPatternDuration();
}

LIGHT_LAYER_TEMPLATE
inline int32_t LIGHT_LAYER_CLASS::getSelectedTransitionDuration() {
	if (activePattern->getTransitionDuration() == -1) {
		if (patternSequence) {
			auto cue = patternSequence->getPatternCue(patternIndex);

			if (cue.transitionDuration == -1) {
				return config.transitionDuration;
			} else {
				return cue.transitionDuration;
			}
		} else {
			return config.transitionDuration;
		}
	}

	return activePattern->getTransitionDuration();
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::updateTransition(uint32_t timeDelta) {
	if (transitionState == TRANSITION_STARTING) {
		transitionState = TRANSITION_RUNNING;
		transitionStartedAt = millis();

		currentTransition = runningBeginTransition ? getSelectedInTransition() : getSelectedOutTransition();
	}

	int32_t transitionDuration = getSelectedTransitionDuration();

	uint32_t timeElapsed = millis() - transitionStartedAt;
	FORMAT ratio = getElapsedTimeRatio();
	bool clear = false;

	switch (currentTransition) {
	case TRANSITION_WIPE:
		timeElapsed = transitionDuration;
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

	case TRANSITION_OVERWRITE:
	default:
		timeElapsed = transitionDuration;
		activePattern->update(timeDelta);
		break;
	}

	if (timeElapsed >= transitionDuration) {
		transitionState = TRANSITION_DONE;
		if (clear && (transitionDuration > 0)) {
			activePattern->getPixelBuffer()->clear();
		}
	}

	if (transitionState == TRANSITION_DONE) {
		if (!runningBeginTransition) {
			switch (config.playMode) {
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
				startSelectedPattern();
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

	int32_t patternTimeDelta = (int32_t)(time - patternStartedAt);
	int32_t patternDuration = getSelectedPatternDuration();

	if (activePattern->isPatternFinished() ||
	        (patternDuration > 0 && patternTimeDelta > patternDuration))
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
