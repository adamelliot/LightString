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

	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
			sections[i].layers[j].pause();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::unpause() {
	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
			sections[i].layers[j].unpause();
		}
	}
}

// ------------------------ Pattern Management ------------------------

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternEventHandler(PatternEvent patternEventHandler) {
	this->layerConfig.patternEventHandler = patternEventHandler;

	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
			sections[i].layers[j].setPatternEventHandler(patternEventHandler);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternEventHandler(PatternEvent patternEventHandler, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].setPatternEventHandler(patternEventHandler);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setMaxPatternDuration(uint32_t maxPatternDuration) {
	this->layerConfig.maxPatternDuration = maxPatternDuration;

	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
			sections[i].layers[j].setMaxPatternDuration(maxPatternDuration);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setMaxPatternDuration(uint32_t maxPatternDuration, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].setTransitionDuration(maxPatternDuration);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setTransitionDuration(uint32_t transitionDuration) {
	this->layerConfig.transitionDuration = transitionDuration;

	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
			sections[i].layers[j].getConfig().transitionDuration = transitionDuration;
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setTransitionDuration(uint32_t transitionDuration, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].getConfig().transitionDuration = transitionDuration;
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPlayMode(EPlayMode playMode) {
	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
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
	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
			if (sections[i].layers[j].isActive()) {
				sections[i].layers[j].startRandomPattern();
			}
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::startRandomPatternOnAllLayers() {
	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
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
	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
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
	for (uint32_t j = 0; j < sections.size(); j++) {
		for (uint32_t i = 0; i < sections[i].getTotalLayers(); i++) {
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
	for (uint32_t j = 0; j < sections.size(); j++) {
		for (uint32_t i = 0; i < sections[i].getTotalLayers(); i++) {
			sections[j].layers[i].prevPattern();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::shufflePatterns() {
	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
			sections[i].layers[j].shufflePatterns();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::ensureLayerIsSetup(uint8_t sectionID, uint8_t layerID) {
	auto &section = sections[sectionID];
	if (section.getTotalLayers() > layerID) return;

	section.ensureLayerExists(layerID);
	auto &layer = section.layers[layerID];

	layer.setConfig(layerConfig);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::addLightPattern(ILightPattern &pattern, uint8_t layerID) {
	for (uint32_t i = 0; i < sections.size(); i++) {
		ensureLayerIsSetup(i, layerID);
		sections[i].layers[layerID].addLightPattern(pattern);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::addLightPattern(ILightPattern &pattern, uint64_t modeList, uint8_t layerID) {
	for (uint32_t i = 0; i < sections.size(); i++) {
		ensureLayerIsSetup(i, layerID);
		sections[i].layers[layerID].addLightPattern(pattern, modeList);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::addLightPattern(ILightPattern &pattern, uint64_t modeList, uint8_t layerID, uint8_t sectionID) {
	ensureLayerIsSetup(sectionID, layerID);
	sections[sectionID].layers[layerID].addLightPattern(pattern, modeList);
}

// -------------------- Adding Sections -------------------

PATTERN_MANAGER_TEMPLATE
LIGHT_SECTION_CLASS *PATTERN_MANAGER_CLASS::getLightSection(uint8_t sectionID) {
	if (sectionID >= sections.size()) return NULL;

	return &sections[sectionID];
}

PATTERN_MANAGER_TEMPLATE
uint8_t PATTERN_MANAGER_CLASS::addLightSection(TPixelBuffer<OUTPUT_PIXEL, FORMAT> &pixelBuffer) {
	auto id = sections.size();

	sections.push_back(LIGHT_SECTION_CLASS());
	sections[id].outputBuffer = &pixelBuffer;

	return sections.size() - 1;
}

PATTERN_MANAGER_TEMPLATE
bool PATTERN_MANAGER_CLASS::addBufferToLightSection(uint8_t sectionID, TPixelBuffer<PIXEL, FORMAT> &buffer) {
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

	for (int i = 0; i < sections.size(); i++) {
		sections[i].update();
	}
	
	return true;
}
