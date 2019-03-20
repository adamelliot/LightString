// ------------------------ Pattern Manager ------------------------

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::pause(bool blackout, bool fade) {
	for (auto &section : m_sections) {
		section->pause(blackout, fade);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::unpause() {
	for (auto &section : m_sections) {
		section->unpause();
	}
}

// ------------------------ Pattern Management ------------------------

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setLayerConfig(const LightLayerConfig &config) {
	this->layerConfig = config;

	for (auto &section : m_sections) {
		for (auto &layer : section->layers()) {
			if (!layer) continue;

			layer->setConfig(config);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setLayerConfig(const LightLayerConfig &config, int layerIndex, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerIndex);
	section->getLayer(layerIndex)->setConfig(config);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternEventHandler(PatternEvent patternEventHandler, void *userData) {
	this->layerConfig.patternEventHandler = patternEventHandler;
	this->layerConfig.patternEventUserData = userData;

	for (auto &section : m_sections) {
		for (auto &layer : section->layers()) {
			if (!layer) continue;
			layer->setPatternEventHandler(patternEventHandler, userData);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternEventHandler(PatternEvent patternEventHandler, void *userData, int layerIndex, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerIndex);
	section->getLayer(layerIndex)->setPatternEventHandler(patternEventHandler, userData);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternDuration(uint32_t patternDuration) {
	this->layerConfig.patternDuration = patternDuration;

	for (auto &section : m_sections) {
		for (auto &layer : section->layers()) {
			if (!layer) continue;

			layer->setPatternDuration(patternDuration);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternDuration(uint32_t patternDuration, int layerIndex, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerIndex);
	section->getLayer(layerIndex)->setTransitionDuration(patternDuration);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setTransitionDuration(uint32_t transitionDuration) {
	this->layerConfig.inTransitionDuration = transitionDuration;
	this->layerConfig.outTransitionDuration = transitionDuration;

	for (auto &section : m_sections) {
		for (auto &layer : section->layers()) {
			if (!layer) continue;
			layer->getConfig().inTransitionDuration = transitionDuration;
			layer->getConfig().outTransitionDuration = transitionDuration;
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setTransitionDuration(uint32_t transitionDuration, int layerIndex, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerIndex);
	section.getLayer(layerIndex)->getConfig().transitionDuration = transitionDuration;
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setBrightness(FORMAT val) {
	for (auto &section : m_sections) {
		section->setBrightness(val);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setBrightness(FORMAT val, uint8_t sectionID) {
	m_sections[sectionID]->setBrightness(val);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPlayMode(EPlayMode playMode) {
	for (auto &section : m_sections) {
		for (auto &layer : section->layers()) {
			if (!layer) continue;
			layer->setPlayMode(playMode);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPlayMode(EPlayMode playMode, int layerIndex, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerIndex);
	section->getLayer(layerIndex)->setPlayMode(playMode);
}

PATTERN_MANAGER_TEMPLATE
bool PATTERN_MANAGER_CLASS::startPattern(PatternCode patternCode, int layerIndex, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerIndex);
	return section->getLayer(layerIndex)->startPattern(patternCode);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::startPattern(uint8_t patternID, int layerIndex) {
	for (auto i = 0; i < m_sections.size(); i++) {
		startPattern(PatternCode(patternID), layerIndex, i);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::startRandomPattern() {
	for (auto &section : m_sections) {
		for (auto &layer : section->layers()) {
			if (!layer || !layer->isActive()) continue;

			layer->startRandomPattern();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::startRandomPatternOnAllLayers() {
	for (auto &section : m_sections) {
		for (auto &layer : section->layers()) {
			if (!layer) continue;
			layer->startRandomPattern();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::startRandomPattern(int layerIndex, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerIndex);
	section->getLayer(layerIndex)->startRandomPattern();
}


PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::play() {
	for (auto &section : m_sections) {
		for (auto &layer : section->layers()) {
			if (!layer) continue;
			layer->play();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::play(int layerIndex, uint8_t sectionID) {
	auto section = getSection(sectionID);
	auto layer = section->getLayer(layerIndex);

	layer->play();
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::nextPattern(bool transition, int layerIndex, uint8_t sectionID) {
	auto section = getSection(sectionID);
	auto layer = section->getLayer(layerIndex);

	layer->nextPattern(transition);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::nextPattern(bool transition) {
	for (auto &section : m_sections) {
		for (auto &layer : section->layers()) {
			if (!layer) continue;

			layer->nextPattern(transition);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::prevPattern(bool transition, int layerIndex, uint8_t sectionID) {
	auto section = getSection(sectionID);
	auto layer = section->getLayer(layerIndex);

	layer->prevPattern(transition);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::prevPattern(bool transition) {
	for (auto &section : m_sections) {
		for (auto &layer : section->layers()) {
			if (!layer) continue;

			layer->prevPattern(transition);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::shufflePatterns() {
	for (auto &section : m_sections) {
		for (auto &layer : section->layers()) {
			if (!layer) continue;

			layer->shufflePatterns();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::ensureLayerIsSetup(LIGHT_SECTION_CLASS &section, int layerIndex) {
	if (section.getLayer(layerIndex) != nullptr) return;

	section.ensureLayerExists(layerIndex);
	auto layer = section.getLayer(layerIndex);

	layer->setConfig(layerConfig);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex, uint32_t flags) {
	for (auto &section : m_sections) {
		for (auto &layer : section->layers()) {
			if (!layer) continue;

			layer->setPatternSequence(patternSequence, newPlayIndex, flags);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex, int layerIndex, uint8_t sectionID, uint32_t flags) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerIndex);
	section->getLayer(layerIndex)->setPatternSequence(patternSequence, newPlayIndex, flags);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::clearPatternSequence(bool fadeOut) {
	for (auto &section : m_sections) {
		for (auto &layer : section->layers()) {
			if (!layer) continue;
			layer->clearPatternSequence();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::clearPatternSequence(int layerIndex, uint8_t sectionID, bool fadeOut) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerIndex);
	section->getLayer(layerIndex)->clearPatternSequence();
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::addLightPattern(pattern_id_t patternID, int layerIndex) {
	for (auto &section : m_sections) {
		ensureLayerIsSetup(*section, layerIndex);
		section->getLayer(layerIndex)->addLightPattern(patternID);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::addLightPattern(pattern_id_t patternID, uint64_t modeList, int layerIndex) {
	for (auto &section : m_sections) {
		ensureLayerIsSetup(*section, layerIndex);
		section->getLayer(layerIndex)->addLightPattern(patternID, modeList);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::addLightPattern(pattern_id_t patternID, uint64_t modeList, int layerIndex, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerIndex);
	section->getLayer(layerIndex)->addLightPattern(patternID, modeList);
}

// -------------------- Adding Sections -------------------

PATTERN_MANAGER_TEMPLATE
LIGHT_SECTION_CLASS *PATTERN_MANAGER_CLASS::getLightSection(uint8_t sectionID) {
	return (sectionID >= m_sections.size()) ? nullptr : m_sections[sectionID].get();
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::clearLightSections() {
	for (auto &section : m_sections) {
		section->clearLayers();	
	}
	m_sections.clear();
}

PATTERN_MANAGER_TEMPLATE
uint8_t PATTERN_MANAGER_CLASS::addLightSection(int size) {
	auto id = m_sections.size();

	m_sections.push_back(std::make_shared<LIGHT_SECTION_CLASS>(patternProvider, size));
	m_sections.back()->setSectionID(id);

	return id;
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

	for (auto &section : m_sections) {
		section->update();
	}

	return true;
}
