// ------------------------ Pattern Manager ------------------------

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::pause(bool blackout, bool fade) {
	for (uint32_t i = 0; i < sections.size(); i++) {
		sections[i].pause(blackout, fade);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::unpause() {
	for (uint32_t i = 0; i < sections.size(); i++) {
		sections[i].unpause();
	}
}

// ------------------------ Pattern Management ------------------------

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setLayerConfig(const LightLayerConfig &config) {
	this->layerConfig = config;

	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
			sections[i].layers[j].setConfig(config);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setLayerConfig(const LightLayerConfig &config, uint8_t layerID, uint8_t sectionID) {
	ensureLayerIsSetup(sectionID, layerID);
	sections[sectionID].layers[layerID].setConfig(config);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternEventHandler(PatternEvent patternEventHandler, void *userData) {
	this->layerConfig.patternEventHandler = patternEventHandler;
	this->layerConfig.patternEventUserData = userData;

	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
			sections[i].layers[j].setPatternEventHandler(patternEventHandler, userData);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternEventHandler(PatternEvent patternEventHandler, void *userData, uint8_t layerID, uint8_t sectionID) {
	ensureLayerIsSetup(sectionID, layerID);
	sections[sectionID].layers[layerID].setPatternEventHandler(patternEventHandler, userData);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternDuration(uint32_t patternDuration) {
	this->layerConfig.patternDuration = patternDuration;

	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
			sections[i].layers[j].setPatternDuration(patternDuration);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternDuration(uint32_t patternDuration, uint8_t layerID, uint8_t sectionID) {
	ensureLayerIsSetup(sectionID, layerID);
	sections[sectionID].layers[layerID].setTransitionDuration(patternDuration);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setTransitionDuration(uint32_t transitionDuration) {
	this->layerConfig.inTransitionDuration = transitionDuration;
	this->layerConfig.outTransitionDuration = transitionDuration;

	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
			sections[i].layers[j].getConfig().inTransitionDuration = transitionDuration;
			sections[i].layers[j].getConfig().outTransitionDuration = transitionDuration;
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setTransitionDuration(uint32_t transitionDuration, uint8_t layerID, uint8_t sectionID) {
	ensureLayerIsSetup(sectionID, layerID);
	sections[sectionID].layers[layerID].getConfig().transitionDuration = transitionDuration;
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setBrightness(FORMAT val) {
	for (uint32_t i = 0; i < sections.size(); i++) {
		sections[i].setBrightness(val);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setBrightness(FORMAT val, uint8_t sectionID) {
	sections[sectionID].setBrightness(val);
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
	ensureLayerIsSetup(sectionID, layerID);
	sections[sectionID].layers[layerID].setPlayMode(playMode);
}

PATTERN_MANAGER_TEMPLATE
bool PATTERN_MANAGER_CLASS::startPattern(PatternCode patternCode, uint8_t layerID, uint8_t sectionID) {
	return sections[sectionID].layers[layerID].startPattern(patternCode);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::startPattern(uint8_t patternID, uint8_t layerID) {
	for (uint32_t i = 0; i < sections.size(); i++) {
		startPattern(PatternCode(patternID), layerID, i);
	}
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
void PATTERN_MANAGER_CLASS::nextPattern(bool transition, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].nextPattern(transition);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::nextPattern(bool transition) {
	for (uint32_t j = 0; j < sections.size(); j++) {
		for (uint32_t i = 0; i < sections[i].getTotalLayers(); i++) {
			sections[j].layers[i].nextPattern(transition);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::prevPattern(bool transition, uint8_t layerID, uint8_t sectionID) {
	sections[sectionID].layers[layerID].prevPattern(transition);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::prevPattern(bool transition) {
	for (uint32_t j = 0; j < sections.size(); j++) {
		for (uint32_t i = 0; i < sections[i].getTotalLayers(); i++) {
			sections[j].layers[i].prevPattern(transition);
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
void PATTERN_MANAGER_CLASS::setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex, bool restartPattern) {
	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
			sections[i].layers[j].setPatternSequence(patternSequence, newPlayIndex, restartPattern);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex, bool restartPattern, uint8_t layerID, uint8_t sectionID) {
	ensureLayerIsSetup(sectionID, layerID);
	sections[sectionID].layers[layerID].setPatternSequence(patternSequence, newPlayIndex, restartPattern);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::clearPatternSequence() {
	for (uint32_t i = 0; i < sections.size(); i++) {
		for (uint32_t j = 0; j < sections[i].getTotalLayers(); j++) {
			sections[i].layers[j].clearPatternSequence();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::clearPatternSequence(uint8_t layerID, uint8_t sectionID) {
	ensureLayerIsSetup(sectionID, layerID);
	sections[sectionID].layers[layerID].clearPatternSequence();
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::addLightPattern(pattern_id_t patternID, uint8_t layerID) {
	for (uint32_t i = 0; i < sections.size(); i++) {
		ensureLayerIsSetup(i, layerID);
		sections[i].layers[layerID].addLightPattern(patternID);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::addLightPattern(pattern_id_t patternID, uint64_t modeList, uint8_t layerID) {
	for (uint32_t i = 0; i < sections.size(); i++) {
		ensureLayerIsSetup(i, layerID);
		sections[i].layers[layerID].addLightPattern(patternID, modeList);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::addLightPattern(pattern_id_t patternID, uint64_t modeList, uint8_t layerID, uint8_t sectionID) {
	ensureLayerIsSetup(sectionID, layerID);
	sections[sectionID].layers[layerID].addLightPattern(patternID, modeList);
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

	sections.push_back(LIGHT_SECTION_CLASS(patternProvider));
	sections[id].setSectionID(id);
	sections[id].outputBuffer = &pixelBuffer;

	return sections.size() - 1;
}

PATTERN_MANAGER_TEMPLATE
bool PATTERN_MANAGER_CLASS::addBufferToLightSection(uint8_t sectionID, TPixelBuffer<PIXEL, FORMAT> &buffer) {
	LIGHT_SECTION_CLASS *lightSection = getLightSection(sectionID);
	if (!lightSection) return false;

	return lightSection->addBuffer(&buffer);
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

	for (int i = 0; i < sections.size(); i++) {
		sections[i].update();
	}

	return true;
}
