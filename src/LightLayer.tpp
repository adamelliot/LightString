LIGHT_LAYER_TEMPLATE
PatternCode LIGHT_LAYER_CLASS::getPatternCodeFromIndex(uint8_t index) {
	if (patternSequence) {
		return patternSequence->getPatternCode(index);
	} else {
		return PatternCode::noPattern();
	}
}

LIGHT_LAYER_TEMPLATE
int LIGHT_LAYER_CLASS::getPlaybackCount() const {
	if (patternSequence) {
		return patternSequence->size();
	} else {
		return 0;
	}
}

/**
 * Set the new Pattern Sequence.
 * @param playIndex index we are going to start playing at, defaults to 0
 * @param restartPattern If the pattern is already playing this flag will cause
 *   the pattern to restart, otherwise playback will just continue for the pattern.
 * @param flags A combination of `ESetPatternSequenceFlags`
 */
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex, uint32_t flags) {

	auto currentCode = PatternCode::anyPattern();
	bool patternChanged = false;
	bool playFromSequence = newPlayIndex >= 0;

	bool retainPattern = (flags & RETAIN_PATTERN);
	bool fadeOut = (flags & FADE_OUT);
	bool shouldTransition = (flags & TRANSITION_PATTERN);
	bool dontStop = (flags & DONT_STOP_CURRENT);

	if (patternSequence.size() == 0) {
		clearPatternSequence(fadeOut, !dontStop);
		return;
	}

	if (this->patternSequence && (patternIndex < this->patternSequence->size())) {
		currentCode = this->patternSequence->getPatternCode(patternIndex);
	}

	this->patternSequence = std::shared_ptr<IPatternSequence>(new PatternSequence(patternSequence));

	if (playFromSequence && newPlayIndex < this->patternSequence->size()) {
		patternIndex = newPlayIndex;
	} else {
		patternIndex = 0;
	}

	if (!playFromSequence) {
		if (!dontStop) stop(fadeOut);
	} else {
		auto newCode = this->patternSequence->getPatternCode(patternIndex);
		if (newCode != currentCode) {
			patternChanged = true;
		}
		if (!retainPattern) patternChanged = true;

		if (patternChanged) {
			startPatternAtIndex(patternIndex, shouldTransition);
		}
	}
}

/**
 * Clears the pattern sequence and stops the current playing pattern if it's from the sequence
 * @param fadeOut set this to have the stop command issued with a fade out
 * @param stopIfPlayingFromSequence If you want to leave the current pattern
 *		playing even after the sequence is gone, set this to `false`
 */
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::clearPatternSequence(bool fadeOut, bool stopIfPlayingFromSequence) {
	if (isRunningPatternFromSequence() && stopIfPlayingFromSequence) {
		stop(fadeOut);
	}

	this->patternSequence.reset();
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
		if (isPaused()) {
			playOutAction = FREEZE_FADE_TO_STOP;
			unpause();
		} else {
			playOutAction = FADE_TO_STOP;
		}
		setPatternIsFinished();
	} else {
		if (playState == PATTERN_STOPPED) return;
		finishPattern();
		setPlayState(PATTERN_STOPPED);

		setPatternIsFinished();
		runningBeginTransition = true;
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

	pauseStartedAt = 0;
}

/**
 * Causes the current pattern to be held. This prevents the duration to allow the pattern
 * to end and advance to the next pattern. This only HOLDs with reference to duration.
 * Other actions (Next, previous, enqueue, etc) will still cause the pattern to advance.
 * When advancing the hold will stay on the next pattern.
 * `isPatternFinished` will also be respected.
 */
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::hold() {
	if (holdPattern) return;
	holdPattern = true;

	if (config.patternEventHandler) {
		config.patternEventHandler(activePattern, PATTERN_HOLD, config.patternEventUserData);
	}
}

/**
 * Removes the hold state on the current pattern.
 */
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::unhold() {
	if (!holdPattern) return;
	holdPattern = false;

	if (config.patternEventHandler) {
		config.patternEventHandler(activePattern, PATTERN_UNHOLD, config.patternEventUserData);
	}
}

/**
 * Starts the actual pattern in a layer and tells the pattern where to render.
 * @param pattern The pattern that will be started up
 * @param mode Which mode the pattern will be told to start
 * @param config the cue that started this pattern. `nullptr` if not loaded from sequence.
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
 * Starts the pattern at patternIndex
 */
LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::startSelectedPattern() {
	finishPattern();

	PatternCode code;
	PatternCue cue;
	PatternConfig *config = nullptr;

	if (patternSequence) {
		if (patternIndex >= patternSequence->size()) {
			setPlayState(PATTERN_STOPPED);
			return false;
		}

		cue = patternSequence->getPatternCue(patternIndex);
		code = cue.code;
		config = &cue;
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

	playOutAction = LOAD_NEXT;
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
bool LIGHT_LAYER_CLASS::startPattern(PatternCode patternCode, bool transition) {
	if (transition && isActive()) {
		if (isPaused())	unpause();
		enqueuePattern(patternCode);
		return true;
	}

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

	clearPatternSequence(false, false);
	finishPattern();

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
bool LIGHT_LAYER_CLASS::enqueuePatternAtIndex(size_t index, bool waitToFinish) {
	if (!patternSequence || index >= patternSequence->size()) return false;

	enqueuedPatternIndex = index;
	playOutAction = LOAD_ENQUEUED_INDEX;

	if (!waitToFinish) setPatternIsFinished();

	return true;
}

/**
 * Starts the pattern at the set index right away. Skips out transitions.
 * @param transition setting this true will cause the next pattern to switch with transition
 */
LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::startPatternAtIndex(size_t index, bool transition) {
	if (!patternSequence || index >= patternSequence->size()) return false;

	if (transition && isActive()) {
		if (isPaused()) unpause();
		enqueuePatternAtIndex(index, false);
		return true;
	} else {
		patternIndex = index;
		return startSelectedPattern();
	}
}

/**
 * Starts a random pattern in the sequence.
 * @param transition setting this true will cause the next pattern to switch with transition
 */
LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::startRandomPattern(bool transition) {
	uint32_t size = 0;

	if (patternSequence) {
		size = patternSequence->size();
	} else {
		return false;
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
	if (!isRunningPatternFromSequence()) {
		if (playOutAction == LOAD_ENQUEUED_PATTERN) {
			if (isPaused()) stop(false);

			if ((isStopped() || willStop()) && !transition) {
				if (runningBeginTransition) skipInTransition = true;
				skipOutTransition = true;
				transition = true;
			}

			if (transition) {
				setPatternIsFinished();
				return true;
			} else {
				return startPattern(enqueuedPattern);
			}
		} else {
			return false;
		}
	}

	if (isPaused()) unpause();

	if ((isStopped() || willStop()) && !transition) {
		if (runningBeginTransition) skipInTransition = true;
		skipOutTransition = true;
		transition = true;
	}

	if (!willStop()) playOutAction = LOAD_NEXT;

	if (transition) {
		if (patternIsFinished) {
			auto size = patternSequence->size();

			patternIndex++;
			patternIndex %= size;
		} else {
			setPatternIsFinished();
		}

		return true;
	} else {
		auto size = patternSequence->size();

		patternIndex++;
		patternIndex %= size;

		return startSelectedPattern();
	}
}

/**
 * Starts the previous pattern in the sequence.
 * @param transition setting this true will cause the next pattern to switch with transition
 */
LIGHT_LAYER_TEMPLATE
bool LIGHT_LAYER_CLASS::prevPattern(bool transition) {
	if (!isRunningPatternFromSequence()) return false;

	if (isPaused()) unpause();

	if ((isStopped() || willStop()) && !transition) {
		if (runningBeginTransition) skipInTransition = true;
		skipOutTransition = true;
		transition = true;
	}

	if (transition) {
		if (patternIsFinished) {
			auto size = patternSequence->size();
			if (patternIndex == 0) {
				patternIndex = size - 1;
			} else {
				patternIndex--;
			}
		} else {
			setPatternIsFinished();
		}

		if (!willStop()) playOutAction = LOAD_PREVIOUS;
		return true;
	} else {
		uint32_t size = patternSequence->size();

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
 * If we have a patternSequence loaded it shuffles it. Otherwise does nothing
 */
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::shufflePatterns() {
	if (!isRunningPatternFromSequence()) return;

	patternSequence->shuffle();
}

// TODO: This will only support up to 64 pattern modes currently, should really support 256
LIGHT_LAYER_TEMPLATE
void LIGHT_LAYER_CLASS::addLightPattern(pattern_id_t patternID, uint64_t modeList) {
	if (!patternSequence) {
		patternSequence = std::shared_ptr<IPatternSequence>(new SimplePatternSequence(config));
	}

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

		patternSequence->addPatternCode(PatternCode(patternID, mode));
	}
}

/**
 * Add the pattern and all it's modes to the current sequence.
 * Calling this multiple times will add copies to the sequence.
 */
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
	if (!activePattern || activePattern->getOutTransition() == TRANSITION_DEFAULT) {
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
	if (!activePattern || activePattern->getPatternDuration() < 0) {
		if (patternSequence) {
			auto cue = patternSequence->getPatternCue(patternIndex);
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
		if (patternSequence) {
			auto cue = patternSequence->getPatternCue(patternIndex);

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
		if (patternSequence) {
			auto cue = patternSequence->getPatternCue(patternIndex);

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
			} else if (playOutAction == FREEZE_FADE_TO_STOP)  {
				currentTransition = TRANSITION_FREEZE_FADE;
				auto section = getLightSection();
				transitionDuration = section ? section->getFadeDuration() : kDefaultFadeOutDuration;
			}
		}
	}

	uint32_t timeElapsed = millis() - transitionStartedAt;
	FORMAT ratio = getElapsedTimeRatio(transitionDuration);
	bool clear = false;


	if (runningBeginTransition) {
		if (skipInTransition) {
			ratio = 1;
			timeElapsed = transitionDuration;
			skipInTransition = false;
		}
	} else {
		if (skipOutTransition) {
			ratio = 1;
			timeElapsed = transitionDuration;
			skipOutTransition = false;
		}
	}

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
					shouldStop = !(nextCode.isAnyPattern() ? nextPattern() : startPattern(nextCode));
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

				case FADE_TO_STOP:
				case FREEZE_FADE_TO_STOP:
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
	        ((patternDuration > 0 && patternTimeDelta >= adjustedPatternDuration)
	        	&& !holdPattern)) {
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
