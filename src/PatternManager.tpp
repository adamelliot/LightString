// ------------------------ Pattern Manager ------------------------

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::pause(bool blackout, bool fade) {
	if (blackout) {
		if (fade) {
			fadeDown();
		} else {
			setBrightness(0);
		}
	}

	for (uint32_t i = 0; i < sectionCount; i++) {
		for (uint32_t j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].pause();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::unpause() {
	for (uint32_t i = 0; i < sectionCount; i++) {
		for (uint32_t j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].unpause();
		}
	}
}

// ------------------------ Pattern Management ------------------------

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternEventHandler(PatternEvent patternEventHandler) {
	for (uint32_t i = 0; i < sectionCount; i++) {
		for (uint32_t j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].setPatternEventHandler(patternEventHandler);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternEventHandler(PatternEvent patternEventHandler, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].setPatternEventHandler(patternEventHandler);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setMaxPatternLength(uint32_t maxPatternLength) {
	for (uint32_t i = 0; i < sectionCount; i++) {
		for (uint32_t j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].setMaxPatternLength(maxPatternLength);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setMaxPatternLength(uint32_t maxPatternLength, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].setMaxPatternLength(maxPatternLength);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPlayMode(EPlayMode playMode) {
	for (uint32_t i = 0; i < sectionCount; i++) {
		for (uint32_t j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].setPlayMode(playMode);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPlayMode(EPlayMode playMode, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].setPlayMode(playMode);
}

PATTERN_MANAGER_TEMPLATE
bool PATTERN_MANAGER_CLASS::startPattern(PatternCode patternCode, uint8_t layerID, uint8_t sectionID) {
	return sections[sectionID].layers[layerID].startPattern(patternCode);
}

PATTERN_MANAGER_TEMPLATE
bool PATTERN_MANAGER_CLASS::startPattern(uint8_t patternID, uint8_t layerID) {
	return startPattern(PatternCode(patternID), 0, 0);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::startRandomPattern() {
	for (uint32_t i = 0; i < sectionCount; i++) {
		for (uint32_t j = 0; j < MAX_LAYERS; j++) {
			if (sections[i].layers[j].isActive()) {
				sections[i].layers[j].startRandomPattern();
			}
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::startRandomPatternOnAllLayers() {
	for (uint32_t i = 0; i < sectionCount; i++) {
		for (uint32_t j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].startRandomPattern();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::startRandomPattern(uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].startRandomPattern();
}


PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::play() {
	for (uint32_t i = 0; i < sectionCount; i++) {
		for (uint32_t j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].play();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::play(uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].play();
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::nextPattern(uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].nextPattern();
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::nextPattern() {
	for (uint32_t j = 0; j < sectionCount; j++) {
		for (uint32_t i = 0; i < MAX_LAYERS; i++) {
			sections[j].layers[i].nextPattern();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::prevPattern(uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].prevPattern();
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::prevPattern() {
	for (uint32_t j = 0; j < sectionCount; j++) {
		for (uint32_t i = 0; i < MAX_LAYERS; i++) {
			sections[j].layers[i].prevPattern();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::shufflePatterns() {
	for (uint32_t i = 0; i < sectionCount; i++) {
		for (uint32_t j = 0; j < MAX_LAYERS; j++) {
			sections[i].layers[j].shufflePatterns();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::addLightPattern(ILightPattern &pattern, uint8_t layerID) {
	for (uint32_t i = 0; i < sectionCount; i++) {
		sections[i].layers[layerID].addLightPattern(pattern);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::addLightPattern(ILightPattern &pattern, uint64_t modeList, uint8_t layerID) {
	for (uint32_t i = 0; i < sectionCount; i++) {
		sections[i].layers[layerID].addLightPattern(pattern, modeList);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::addLightPattern(ILightPattern &pattern, uint64_t modeList, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].addLightPattern(pattern, modeList);
}

// -------------------- Adding Sections -------------------

PATTERN_MANAGER_TEMPLATE
LIGHT_SECTION_CLASS *PATTERN_MANAGER_CLASS::getLightSection(uint8_t sectionID) {
	if (sectionID >= sectionCount) return NULL;

	return &sections[sectionID];
}

PATTERN_MANAGER_TEMPLATE
uint8_t PATTERN_MANAGER_CLASS::addLightSection(TPixelBuffer<PIXEL, FORMAT> &pixelBuffer) {
	if (sectionCount >= MAX_LIGHT_SECTIONS) {
#ifdef VERBOSE
#ifdef ARDUINO
		Serial.println(F("ERROR: Maximum amount of light sections already added."));
#else
		fprintf(stderr, "ERROR: Maximum amount of light sections already added.");
#endif
#endif
		return 0xff;
	}

	sections[sectionCount].outputBuffer = &pixelBuffer;

	return sectionCount++;
}

PATTERN_MANAGER_TEMPLATE
bool PATTERN_MANAGER_CLASS::addBufferToLightSection(uint8_t sectionID, IPixelBuffer &buffer) {
	LIGHT_SECTION_CLASS *lightSection = getLightSection(sectionID);
	if (!lightSection) return false;

	return lightSection->addBuffer(&buffer);
}

// -------------------- Brightness Control -------------------

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::fadeDown() {
	targetBrightness = 0;
	brightnessStep = -adjustedBrightness / kTransitionFrames;
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::fadeUp(bool forceZero) {
	if (forceZero) {
		adjustedBrightness = 0;
#ifdef USE_FASTLED
		FastLED.setBrightness(adjustedBrightness);
#endif
	}

	targetBrightness = brightness;
	brightnessStep = (brightness - adjustedBrightness) / kTransitionFrames;
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setBrightness(uint8_t brightness) {
	this->brightness = brightness;
	this->adjustedBrightness = brightness;
	this->targetBrightness = brightness;

#ifdef USE_FASTLED
	FastLED.setBrightness(brightness);
#endif
}

PATTERN_MANAGER_TEMPLATE
bool PATTERN_MANAGER_CLASS::isTransitioning() {
	return adjustedBrightness != targetBrightness;
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::transitionBrightness() {
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

PATTERN_MANAGER_TEMPLATE
bool PATTERN_MANAGER_CLASS::update() {
	uint32_t time = millis(), timeDelta = time - lastTime;

	// TODO: Frame rate limiting isn't working quite right
	if (timeDelta < msPerFrame) {
		usleep(1000);
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
