// ------------------------ Pattern Manager ------------------------

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::pause(bool blackout, bool fade) {
	for (auto &section : sections) {
		section->pause(blackout, fade);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::unpause() {
	for (auto &section : sections) {
		section->unpause();
	}
}

// ------------------------ Pattern Management ------------------------

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setLayerConfig(const LightLayerConfig &config) {
	this->layerConfig = config;

	for (auto &section : sections) {
		for (auto &layer : section->layers) {
			if (!layer) continue;

			layer->setConfig(config);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setLayerConfig(const LightLayerConfig &config, uint8_t layerID, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerID);
	section->getLayer(layerID)->setConfig(config);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternEventHandler(PatternEvent patternEventHandler, void *userData) {
	this->layerConfig.patternEventHandler = patternEventHandler;
	this->layerConfig.patternEventUserData = userData;

	for (auto &section : sections) {
		for (auto &layer : section->layers) {
			if (!layer) continue;
			layer->setPatternEventHandler(patternEventHandler, userData);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternEventHandler(PatternEvent patternEventHandler, void *userData, uint8_t layerID, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerID);
	section->getLayer(layerID)->setPatternEventHandler(patternEventHandler, userData);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternDuration(uint32_t patternDuration) {
	this->layerConfig.patternDuration = patternDuration;

	for (auto &section : sections) {
		for (auto &layer : section->layers) {
			if (!layer) continue;

			layer->setPatternDuration(patternDuration);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternDuration(uint32_t patternDuration, uint8_t layerID, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerID);
	section->getLayer(layerID)->setTransitionDuration(patternDuration);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setTransitionDuration(uint32_t transitionDuration) {
	this->layerConfig.inTransitionDuration = transitionDuration;
	this->layerConfig.outTransitionDuration = transitionDuration;

	for (auto &section : sections) {
		for (auto &layer : section->layers) {
			if (!layer) continue;
			layer->getConfig().inTransitionDuration = transitionDuration;
			layer->getConfig().outTransitionDuration = transitionDuration;
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setTransitionDuration(uint32_t transitionDuration, uint8_t layerID, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerID);
	section.getLayer(layerID)->getConfig().transitionDuration = transitionDuration;
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setBrightness(FORMAT val) {
	for (auto &section : sections) {
		section->setBrightness(val);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setBrightness(FORMAT val, uint8_t sectionID) {
	auto section = getSection(sectionID);
	section->setBrightness(val);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPlayMode(EPlayMode playMode) {
	for (auto &section : sections) {
		for (auto &layer : sections->layers) {
			if (!layer) continue;
			layer->setPlayMode(playMode);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPlayMode(EPlayMode playMode, uint8_t layerID, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerID);
	section->getLayer(layerID)->setPlayMode(playMode);
}

PATTERN_MANAGER_TEMPLATE
bool PATTERN_MANAGER_CLASS::startPattern(PatternCode patternCode, uint8_t layerID, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerID);
	return section->getLayer(layerID)->startPattern(patternCode);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::startPattern(uint8_t patternID, uint8_t layerID) {
	for (auto i = 0; i < sections.size(); i++) {
		startPattern(PatternCode(patternID), layerID, i);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::startRandomPattern() {
	for (auto &section : sections) {
		for (auto &layer : section->layers) {
			if (!layer || !layer->isActive()) continue;

			layer->startRandomPattern();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::startRandomPatternOnAllLayers() {
	for (auto &section : sections) {
		for (auto &layer : section->layers) {
			if (!layer) continue;
			layer->startRandomPattern();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::startRandomPattern(uint8_t layerID, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerID);
	section->getLayer(layerID)->startRandomPattern();
}


PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::play() {
	for (auto &section : sections) {
		for (auto &layer : section->layers) {
			if (!layer) continue;
			layer->play();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::play(uint8_t layerID, uint8_t sectionID) {
	auto section = getSection(sectionID);
	auto layer = section->getLayer(layerID);

	layer->play();
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::nextPattern(bool transition, uint8_t layerID, uint8_t sectionID) {
	auto section = getSection(sectionID);
	auto layer = section->getLayer(layerID);

	layer->nextPattern(transition);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::nextPattern(bool transition) {
	for (auto &section : sections) {
		for (auto &layer : section->layers) {
			if (!layer) continue;

			layer->nextPattern(transition);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::prevPattern(bool transition, uint8_t layerID, uint8_t sectionID) {
	auto section = getSection(sectionID);
	auto layer = section->getLayer(layerID);

	layer->prevPattern(transition);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::prevPattern(bool transition) {
	for (auto &section : sections) {
		for (auto &layer : section->layers) {
			if (!layer) continue;

			layer->prevPattern(transition);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::shufflePatterns() {
	for (auto &section : sections) {
		for (auto &layer : section->layers) {
			if (!layer) continue;

			layer->shufflePatterns();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::ensureLayerIsSetup(LIGHT_SECTION_CLASS &section, uint8_t layerID) {
	if (section.getLayer(layerID) != nullptr) return;

	section.ensureLayerExists(layerID);
	auto layer = section.getLayer(layerID);

	layer->setConfig(layerConfig);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex, uint32_t flags) {
	for (auto &section : sections) {
		for (auto &layer : section->layers) {
			if (!layer) continue;

			layer->setPatternSequence(patternSequence, newPlayIndex, flags);
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::setPatternSequence(const PatternSequence &patternSequence, int newPlayIndex, uint8_t layerID, uint8_t sectionID, uint32_t flags) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerID);
	section->getLayer(layerID)->setPatternSequence(patternSequence, newPlayIndex, flags);
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::clearPatternSequence(bool fadeOut) {
	for (auto &section : sections) {
		for (auto &layer : section->layers) {
			if (!layer) continue;
			layer->clearPatternSequence();
		}
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::clearPatternSequence(uint8_t layerID, uint8_t sectionID, bool fadeOut) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerID);
	section->getLayer(layerID)->clearPatternSequence();
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::addLightPattern(pattern_id_t patternID, uint8_t layerID) {
	for (auto &section : sections) {
		ensureLayerIsSetup(*section, layerID);
		section->getLayer(layerID)->addLightPattern(patternID);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::addLightPattern(pattern_id_t patternID, uint64_t modeList, uint8_t layerID) {
	for (auto &section : sections) {
		ensureLayerIsSetup(*section, layerID);
		section->getLayer(layerID)->addLightPattern(patternID, modeList);
	}
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::addLightPattern(pattern_id_t patternID, uint64_t modeList, uint8_t layerID, uint8_t sectionID) {
	auto section = getSection(sectionID);
	ensureLayerIsSetup(*section, layerID);
	section->getLayer(layerID)->addLightPattern(patternID, modeList);
}

// -------------------- Adding Sections -------------------

PATTERN_MANAGER_TEMPLATE
LIGHT_SECTION_CLASS *PATTERN_MANAGER_CLASS::getLightSection(uint8_t sectionID) {
	return (sectionID >= sections.size()) ? nullptr : sections[sectionID].get();
}

PATTERN_MANAGER_TEMPLATE
void PATTERN_MANAGER_CLASS::clearLightSections() {
	for (auto &section : sections) {
		section->clearLayers();	
	}
	sections.clear();
}

PATTERN_MANAGER_TEMPLATE
uint8_t PATTERN_MANAGER_CLASS::addLightSection(TPixelBuffer<OUTPUT_PIXEL, FORMAT> &pixelBuffer) {
	auto id = sections.size();

	sections.push_back(std::make_shared<LIGHT_SECTION_CLASS>(patternProvider));

	sections[id]->setSectionID(id);
	sections[id]->outputBuffer = &pixelBuffer;

	return id;
}

PATTERN_MANAGER_TEMPLATE
bool PATTERN_MANAGER_CLASS::addBufferToLightSection(uint8_t sectionID, TPixelBuffer<PIXEL, FORMAT> &buffer) {
	auto section = getSection(sectionID);
	return section->addBuffer(&buffer);
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

	for (auto &section : sections) {
		section->update();
	}

	return true;
}
