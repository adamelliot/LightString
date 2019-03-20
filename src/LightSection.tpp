LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::resize(int size) {
	outputBuffer.resize(size);
	m_bufferProvider.setBufferSize(size);

	for (auto &layer : m_layers) {
		auto buffer = layer->getPixelBuffer();
		if (buffer)
			buffer->resize(size);
	}
}

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::ensureLayerExists(int layerIndex) {
	if (layerIndex < m_layers.size() && m_layers[layerIndex] != nullptr) return;

	if (layerIndex >= m_layers.size()) {
		m_layers.resize(layerIndex + 1);
	}

	for (auto id = m_layers.size() - 1; id <= layerIndex; id++) {
		m_layers[id] = std::make_shared<LightLayer<PIXEL, FORMAT>>(patternProvider, m_bufferProvider);
		m_layers[id]->setLightSection(this);
	}
}

LIGHT_SECTION_TEMPLATE
int LIGHT_SECTION_CLASS::indexOfLayer(ILightLayer *layer) {
	for (int i = 0; i < m_layers.size(); i++)
	{
		if (m_layers[i].get() == layer) return i;
	}

	return -1;
}

LIGHT_SECTION_TEMPLATE
bool LIGHT_SECTION_CLASS::startPatternOnNewLayer(PatternCode patternCode, LightLayerConfig config) {
	m_layers.push_back(std::make_shared<LightLayer<PIXEL, FORMAT>>(patternProvider, m_bufferProvider));
	auto & layer = m_layers.back();

	layer->setConfig(config);
	layer->setLightSection(this);

	return layer->startPattern(patternCode);
}

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::stopAllLayers(bool fadeOut) {
	for (auto & layer : m_layers)
		layer->stop(fadeOut);

	pruneStoppedLayers();
}

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::startOutTransitionOnAllLayers() {
	for (auto & layer : m_layers)
		layer->startOutTransition();
}

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::pruneStoppedLayers() {
	for (auto it = m_layers.begin(); it != m_layers.end();)
	{
		if (*it == nullptr || (*it)->getActivePattern() == nullptr)
			m_layers.erase(it);
		else
			++it;
	}
}

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::playAllLayers() {
	for (auto & layer : m_layers)
		layer->play();
}

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::update() {
	pruneStoppedLayers();

	outputBuffer.clear();

	int i = 0;
	for (auto &layer : m_layers) {
		layer->update();

		auto pattern = layer->getActivePattern();

		if (pattern && !pattern->isFilterPattern()) {
			auto buffer = layer->getPixelBuffer();

			FORMAT opacity = TColorFormatHelper<FORMAT>::scale(layer->getOpacity(), layer->getTransitionOpacity());

			if (opacity < layer->getMaxOpacity()) {
				if (opacity > 0) {
					outputBuffer.blendWith(*buffer, pattern->getBlendMode(), opacity);
				}
			} else {
				outputBuffer.blendWith(*buffer, pattern->getBlendMode());
			}
		}

		i++;
	}

	updateFadeLevel();

	FORMAT scaledLevel = TColorFormatHelper<FORMAT>::scale(brightness, fade);

	if (scaledLevel < TColorFormatHelper<FORMAT>::getMaxValue()) {
		outputBuffer.fade(scaledLevel);
	}
}

// -------------------- Play Control -------------------

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::pause(bool blackout, bool fade) {
	if (blackout) {
		if (fade) {
			fadeDown();
			pauseAfterFade = true;
		} else {
			setBrightness(0);
			for (auto &layer : m_layers) {
				if (!layer) continue;
				layer->pause();
			}
		}
	} else {
		if (!fade) {
			for (auto &layer : m_layers) {
				if (!layer) continue;
				layer->pause();
			}
		}
	}
}

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::unpause(bool fade) {
	if (fade) fadeUp();

	for (auto &layer : m_layers) {
		if (!layer) continue;
		layer->unpause();
	}
}

// -------------------- Brightness Control -------------------

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::fadeDown() {
	targetFade = 0;
	fadeEnd = millis() + fadeDuration;
}

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::fadeUp(bool forceZero) {
	if (forceZero) {
		fade = 0;
	}

	targetFade = TColorFormatHelper<FORMAT>::getMaxValue();
	fadeEnd = millis() + fadeDuration;
}

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::updateFadeLevel() {
	if (fade == targetFade) {
		if (pauseAfterFade) {
			pauseAfterFade = false;
			pause(false, false);
		}
		return;
	}

	uint32_t fadeStart = fadeEnd - fadeDuration;
	uint32_t t = millis() - fadeStart;
	FORMAT ratio;

	if (t >= fadeDuration) {
		ratio = TColorFormatHelper<FORMAT>::getMaxValue();
	} else {
		ratio = (t * TColorFormatHelper<FORMAT>::getMaxValue()) / fadeDuration;
	}

	if (targetFade == 0) {
		fade = TColorFormatHelper<FORMAT>::getMaxValue() - ratio;
	} else {
		fade = ratio;
	}
}
