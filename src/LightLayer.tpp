LIGHT_LAYER_TEMPLATE
PatternCode LIGHT_LAYER_CLASS::getPatternCodeFromIndex(uint8_t index) {
	if (hasPatternSequence) {
		return patternSequence.getSequence()[index].code;
	} else {
		return patternList[index];
	}
}

LIGHT_LAYER_TEMPLATE
int LIGHT_LAYER_CLASS::getPlaybackCount() const {
	if (hasPatternSequence) {
		return patternSequence.getSequence().size();
	} else {
		return patternList.size();
	}
}

/**
 * Set the new Pattern Sequence
 * @param playIndex index we are going to start playing at, defaults to 0
 * @param restartPattern If the pattern is already playing this flag will cause the pattern to restart, otherwise
 *   playback will just continue for the pattern.
 */
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex, bool restartPattern, bool fadeOut) {
	auto currentCode = PatternCode(0xff, 0xff);
	bool patternChanged = false;

	bool playFromSequence = newPlayIndex >= 0;

	if (hasPatternSequence && (patternIndex < this->patternSequence.getSequence().size())) {
		currentCode = this->patternSequence.getPatternCue(patternIndex).code;
	}

	this->patternSequence = patternSequence;
	hasPatternSequence = true;

	patternIndex = newPlayIndex;

	if (patternIndex >= this->patternSequence.getSequence().size() || !playFromSequence) {
		patternIndex = 0;
	}

	if (!playFromSequence && runningPatternFromSequence) {
		stop(fadeOut);
	} else {
		if (this->patternSequence.getSequence().size() > 0) {
			auto newCode = this->patternSequence.getPatternCue(patternIndex).code;
			if (newCode != currentCode) {
				patternChanged = true;
			}
		} else {
			stop(fadeOut);
			return;
		}

		if (restartPattern) patternChanged = true;

		if (patternChanged && isActive()) {
			startSelectedPattern();
		}
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::clearPatternSequence(bool fadeOut) {
	this->patternSequence.getSequence().clear();
	hasPatternSequence = false;

	if (runningPatternFromSequence) {
		stop(fadeOut);
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::setPatternEventHandler(PatternEvent patternEventHandler, void *userData) {
	this->config.patternEventHandler = patternEventHandler;
	this->config.patternEventUserData = userData;
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::finishPattern() {
	if (!activePattern) return;

	setPlayState(PATTERN_FINISHED);
	activePattern->patternFinished();

	if (!activePattern->isFilterPattern() && section != nullptr) {
		section->unlockBuffer(activePattern->getPixelBuffer());
	}

	if (this->activePattern != nullptr) {
		patternProvider.finishedWithPattern(this->activePattern);
		this->activePattern = nullptr;
	}
}

LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::suspendPattern() {
	if (!activePattern || !runningPatternFromSequence) return false;

	suspendedPattern = activePattern;
	runningPatternFromSequence = false;
	activePattern = nullptr;

	suspendedPatternStartedAt = patternStartedAt;
	suspendStartedAt = millis();

	if (config.patternEventHandler) {
		config.patternEventHandler(suspendedPattern, PATTERN_SUSPENDED, config.patternEventUserData);
	}

	return true;
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::finalizeSuspendedPattern() {
	if (!suspendedPattern) return;

	if (config.patternEventHandler) {
		config.patternEventHandler(suspendedPattern, PATTERN_FINISHED, config.patternEventUserData);
	}

	patternProvider.finishedWithPattern(suspendedPattern);
	suspendedPattern = nullptr;
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::setPlayState(EPlayState playState) {
	this->playState = playState;

	if (config.patternEventHandler) {
		config.patternEventHandler(activePattern, playState, config.patternEventUserData);
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::play() {
	if (playState == PATTERN_PLAYING ||
	        playState == PATTERN_PLAYING_IN_TRANSITION ||
	        playState == PATTERN_PLAYING_OUT_TRANSITION) return;
	if (playState == PATTERN_PAUSED) {
		unpause();
		return;
	}

	startSelectedPattern();
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::stop(bool fadeOut) {
	if (fadeOut) {
		playOutAction = FADE_TO_STOP;
		setPatternIsFinished();
	} else {
		if (playState == PATTERN_STOPPED) return;
		finishPattern();
		setPlayState(PATTERN_STOPPED);
	}
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

	if (transitionState != TRANSITION_DONE) {
		setPlayState(runningBeginTransition ? PATTERN_PLAYING_IN_TRANSITION : PATTERN_PLAYING_OUT_TRANSITION);
	} else {
		setPlayState(PATTERN_PLAYING);
	}

	uint32_t timeDelta = millis() - pauseStartedAt;
	patternStartedAt += timeDelta;
	transitionStartedAt += timeDelta;
}

/**
 * Starts the actual pattern in a layer and tells the pattern where to render.
 * @param pattern The pattern that will be started up
 * @param mode Which mode the pattern will be told to start
 * @param config the cue that started this pattern. `nullptr` if not loaded from sequence
 */
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::startPattern(ILightPattern *pattern, uint8_t mode, PatternConfig *config) {
	this->transitionOpacity = getMaxOpacity();

	this->activePattern = pattern;
	this->activePattern->setLayer(this);

	IPixelBuffer *buffer = nullptr;

	if (this->section) {
		if (this->activePattern->isFilterPattern()) {
			buffer = this->section->getOutputBuffer();
		} else {
			buffer = this->section->lockBuffer();
		}
	}

	if (!buffer) {
		finishPattern();
		setPlayState(PATTERN_STOPPED);
		return;
	}

	this->activePattern->setPixelBuffer(buffer);

	setPlayState(PATTERN_SETUP);

	this->activePattern->setMode(mode, config);

	setPlayState(PATTERN_STARTED);

	patternIsFinished = false;
	this->patternStartedAt = millis();
	this->transitionState = TRANSITION_STARTING;

	runningBeginTransition = true;
}

/**
 * Starts the pattern at patternIndex and chooses between sequence or patternList.
 */
LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::startSelectedPattern() {
	finishPattern();

	PatternCode code;
	PatternConfig *config = nullptr;

	if (hasPatternSequence) {
		if (patternIndex >= patternSequence.getSequence().size()) {
			setPlayState(PATTERN_STOPPED);
			return false;
		}

		auto &cue = patternSequence.getPatternCue(patternIndex);
		code = cue.code;
		config = &cue;
	} else if (patternIndex < patternList.size()) {
		code = patternList[patternIndex];
	} else {
		setPlayState(PATTERN_STOPPED);
		return false;
	}

	auto pattern = patternProvider.patternForID(code.patternID, this);

	if (!pattern) {
#ifdef __DISPLAY_ERROR
		Serial.print("Pattern not found: 0x");
		Serial.print(cue.code.patternID, HEX);
		Serial.print(" on Layer: ");
		Serial.println(layerID);
#endif
		return false;
	} else {
		pattern->setPatternID(code.patternID);
	}

	finalizeSuspendedPattern();
	runningPatternFromSequence = true;

	startPattern(pattern, code.mode, config);
	return true;
}

/**
 * Starts the pattern with the specified code next and lets the current run its out transition
 * @param patternCode The pattern code to load
 * @param waitToFinish set this to true to have the current pattern finish it's full
 *   play duration before loading the next one.
 */
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::enqueuePattern(PatternCode patternCode, bool waitToFinish) {
	enqueuedPattern = patternCode;
	playOutAction = LOAD_ENQUEUED_PATTERN;

	if (!waitToFinish) setPatternIsFinished();
}

/**
 * Starts the pattern with the specified code.
 * @param patternCode The pattern code to load
 */
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

	Serial.print(F("\tLayer: "));
	Serial.println(layerID);
#else
	printf("Starting Pattern: 0x%x\tMode: ", patternCode.patternID);
	if (patternCode.mode == ALL_MODES) {
		printf("R");
	} else {
		printf("%d", patternCode.mode);
	}
	printf("\tLayer: %d\n", layerID);
#endif
#endif

	bool randomMode = patternCode.mode == 0xff;
	if (randomMode) patternCode.mode = 0;

	ILightPattern *pattern = patternProvider.patternForID(patternCode.patternID, this);
	if (!pattern) {
#ifdef __DISPLAY_ERROR
		Serial.print("Pattern not found: 0x");
		Serial.print(patternCode.patternID, HEX);
		Serial.print(" on Layer: ");
		Serial.println(layerID);
#endif
		return false;
	} else {
		pattern->setPatternID(patternCode.patternID);
	}

	if (!suspendPattern()) {
		finishPattern();
	}

	if (randomMode) {
		patternCode.mode = random(pattern->getModeCount());
	}

	startPattern(pattern, patternCode.mode);

	return true;
}

/**
 * Enqueues a pattern to start after the current finishes it's transition
 * @param index the pattern index that will be loaded next
 * @param waitToFinish set this to true to have the current pattern finish it's full
 *   play duration before loading the next one.
 */
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::enqueuePatternAtIndex(int index, bool waitToFinish) {
	enqueuedPatternIndex = index;
	playOutAction = LOAD_ENQUEUED_INDEX;

	if (!waitToFinish) setPatternIsFinished();
}

/**
 * Starts the pattern at the set index right away. Skips out transitions.
 * @param transition setting this true will cause the next pattern to switch with transition
 */
LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::startPatternAtIndex(int index) {
	patternIndex = index;
	return startSelectedPattern();
}

/**
 * Starts a random pattern in the sequence.
 * @param transition setting this true will cause the next pattern to switch with transition
 */
LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::startRandomPattern(bool transition) {
	uint32_t size = patternList.size();

	if (hasPatternSequence) {
		size = patternSequence.getSequence().size();
	}

	auto index = random(size);

	if (transition) {
		enqueuePatternAtIndex(index);
		return true;
	} else {
		patternIndex = index;
		return startSelectedPattern();
	}
}

/**
 * Starts the next pattern in the sequence.
 * @param transition setting this true will cause the next pattern to switch with transition
 */
LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::nextPattern(bool transition) {
	playOutAction = LOAD_NEXT;

	if (transition) {
		setPatternIsFinished();
		return true;
	} else {
		uint32_t size = patternList.size();

		if (hasPatternSequence) {
			size = patternSequence.getSequence().size();
		}

		patternIndex++;
		if (size > 0) patternIndex %= size;

		return startSelectedPattern();
	}
}

/**
 * Starts the previous pattern in the sequence.
 * @param transition setting this true will cause the next pattern to switch with transition
 */
LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::prevPattern(bool transition) {
	if (transition) {
		setPatternIsFinished();
		playOutAction = LOAD_PREVIOUS;
		return true;
	} else {
		uint32_t size = patternList.size();

		if (hasPatternSequence) {
			size = patternSequence.getSequence().size();
		}
		if (patternIndex == 0) {
			patternIndex = size - 1;
		} else {
			patternIndex--;
		}

		playOutAction = LOAD_NEXT;
		return startSelectedPattern();
	}
}

/**
 * If there is a suspended pattern it will resume playback.
 * @param transition setting this true will cause the suspended pattern to switch with transition
 */
LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::resumeSequence(bool transition) {
	if (!suspendedPattern) return false;

	runningPatternFromSequence = true;

	if (transition) {
		setPatternIsFinished();
		playOutAction = LOAD_SUSPENDED_PATTERN;
		return true;
	} else {
		finishPattern();
		activePattern = suspendedPattern;

		uint32_t timeDelta = millis() - suspendStartedAt;
		patternIsFinished = false;
		patternStartedAt = suspendedPatternStartedAt;
		patternStartedAt += timeDelta;

		suspendedPattern = nullptr;
		if (config.patternEventHandler) {
			config.patternEventHandler(activePattern, PATTERN_RESUMED, config.patternEventUserData);
		}

		playOutAction = LOAD_NEXT;

		this->transitionState = TRANSITION_DONE;
		runningBeginTransition = false;
		setPlayState(PATTERN_PLAYING);
	}

	return true;
}

/**
 * Shuffles all the Patterns in the patternList. Does nothing to the sequence
 */
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::shufflePatterns() {
	auto size = patternList.size();
	if (size <= 1) return;

	for (size_t i = 0; i < patternList.size(); i++) {
		size_t j = (i + random() / (0x7ffffff / (patternList.size() - i) + 1)) % patternList.size();
		PatternCode t = patternList[j];
		patternList[j] = patternList[i];
		patternList[i] = t;
	}
}

// TODO: This will only support up to 64 pattern modes currently, should really support 256
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::addLightPattern(pattern_id_t patternID, uint64_t modeList) {
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

		patternList.push_back(PatternCode(patternID, mode));
	}
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::addLightPattern(pattern_id_t patternID) {
	uint64_t modeList = 0;
	auto pattern = patternProvider.patternForID(patternID);
	if (!pattern) return;

	for (uint32_t i = 0; i < pattern->getModeCount(); i++) {
		modeList |= 1 << i;
	}

	patternProvider.finishedWithPattern(pattern);

	addLightPattern(patternID, modeList);
}

template <>
inline float LightLayer<float>::getElapsedTimeRatio(int32_t transitionDuration) {
	if (transitionDuration <= 0) return 1.0f;

	uint32_t timeElapsed = millis() - transitionStartedAt;
	float ratio = (float)timeElapsed / (float)transitionDuration;
	if (ratio > 1.0f) ratio = 1.0f;

	return ratio;
}

template <>
inline uint8_t LightLayer<uint8_t>::getElapsedTimeRatio(int32_t transitionDuration) {
	if (transitionDuration <= 0) return 255;

	uint32_t timeElapsed = millis() - transitionStartedAt;
	uint32_t ratio = timeElapsed * 256 / transitionDuration;
	if (ratio > 255) ratio = 255;
	return (uint8_t)ratio;
}

template <>
inline uint8_t LightLayer<uint8_t>::getMaxOpacity() { return 0xff; }

template <>
inline float LightLayer<float>::getMaxOpacity() { return 1.0f; }

LIGHT_LAYER_TEMPLATE
inline EPatternTransition LIGHT_LAYER_CLASS::getSelectedInTransition() {
	if (!activePattern || activePattern->getInTransition() == TRANSITION_DEFAULT) {
		if (hasPatternSequence) {
			auto cue = patternSequence.getPatternCue(patternIndex);
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
	if (!activePattern || activePattern->getOutTransition() == TRANSITION_DEFAULT) {
		if (hasPatternSequence) {
			auto cue = patternSequence.getPatternCue(patternIndex);
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
	if (!activePattern || activePattern->getPatternDuration() < 0) {
		if (hasPatternSequence) {
			auto cue = patternSequence.getPatternCue(patternIndex);
			if (cue.patternDuration < 0) {
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
inline int32_t LIGHT_LAYER_CLASS::getSelectedInTransitionDuration() {
	if (!activePattern || activePattern->getInTransitionDuration() < 0) {
		if (hasPatternSequence) {
			auto cue = patternSequence.getPatternCue(patternIndex);

			if (cue.inTransitionDuration < 0) {
				return config.inTransitionDuration;
			} else {
				return cue.inTransitionDuration;
			}
		} else {
			return config.inTransitionDuration;
		}
	}

	return activePattern->getInTransitionDuration();
}

LIGHT_LAYER_TEMPLATE
inline int32_t LIGHT_LAYER_CLASS::getSelectedOutTransitionDuration() {
	if (!activePattern || activePattern->getOutTransitionDuration() < 0) {
		if (hasPatternSequence) {
			auto cue = patternSequence.getPatternCue(patternIndex);

			if (cue.outTransitionDuration < 0) {
				return config.outTransitionDuration;
			} else {
				return cue.outTransitionDuration;
			}
		} else {
			return config.outTransitionDuration;
		}
	}

	return activePattern->getOutTransitionDuration();
}

LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::updateTransition(uint32_t timeDelta) {
	int32_t transitionDuration = runningBeginTransition ? getSelectedInTransitionDuration() : getSelectedOutTransitionDuration();

	if (transitionState == TRANSITION_STARTING) {
		transitionState = TRANSITION_RUNNING;
		transitionStartedAt = millis();

		if (runningBeginTransition) {
			currentTransition = getSelectedInTransition();
			setPlayState(PATTERN_PLAYING_IN_TRANSITION);
		} else {
			currentTransition = getSelectedOutTransition();
			setPlayState(PATTERN_PLAYING_OUT_TRANSITION);

			if (playOutAction == FADE_TO_STOP) {
				currentTransition = TRANSITION_FADE_DOWN;
				auto section = getLightSection();
				transitionDuration = section ? section->getFadeDuration() : kDefaultFadeOutDuration;
			}
		}
	}

	uint32_t timeElapsed = millis() - transitionStartedAt;
	FORMAT ratio = getElapsedTimeRatio(transitionDuration);
	bool clear = false;

	switch (currentTransition) {
	case TRANSITION_WIPE:
		activePattern->update(timeDelta);
		clear = true;
		break;

	case TRANSITION_FREEZE_FADE:
		transitionOpacity = getMaxOpacity() - ratio;
		clear = true;
		break;

	case TRANSITION_FADE_DOWN:
		activePattern->update(timeDelta);
		transitionOpacity = getMaxOpacity() - ratio;
		clear = true;
		break;

	case TRANSITION_FADE_UP:
		activePattern->update(timeDelta);
		transitionOpacity = ratio;
		break;

	case TRANSITION_OVERWRITE:
	default:
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
				PatternCode nextCode = activePattern->getNextPatternCode();
				bool shouldStop = false;

				switch (playOutAction) {
				case LOAD_NEXT:
					shouldStop = !(nextCode.isAnyCode() ? nextPattern() : startPattern(nextCode));
					break;

				case LOAD_PREVIOUS:
					shouldStop = !prevPattern();
					break;

				case LOAD_ENQUEUED_PATTERN:
					shouldStop = !startPattern(enqueuedPattern);
					break;

				case LOAD_ENQUEUED_INDEX:
					shouldStop = !startPatternAtIndex(enqueuedPatternIndex);
					break;

				case LOAD_SUSPENDED_PATTERN:
					shouldStop = !resumeSequence();
					break;

				case FADE_TO_STOP:
					shouldStop = true;
					break;
				}

				if (shouldStop) {
					finishPattern();
					setPlayState(PATTERN_STOPPED);
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

			playOutAction = LOAD_NEXT;
		} else {
			setPlayState(PATTERN_PLAYING);
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
	// Pattern time until the out transition should start.
	auto adjustedPatternDuration = patternDuration - getSelectedOutTransitionDuration();

	if (activePattern->isPatternFinished() || 
		(patternDuration > 0 && patternTimeDelta >= adjustedPatternDuration)) {
		setPatternIsFinished();
	}

	if (patternIsFinished && transitionState == TRANSITION_DONE) {
		transitionState = TRANSITION_STARTING;
	}

	if (transitionState != TRANSITION_DONE) {
		updateTransition(timeDelta);
	} else {
		activePattern->update(timeDelta);
	}
}
