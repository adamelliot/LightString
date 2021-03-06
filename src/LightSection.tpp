LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::ensureLayerExists(uint8_t layerID) {
	if (layerID < layers.size() && layers[layerID] != nullptr) return;

	if (layerID >= layers.size()) {
		layers.resize(layerID + 1);
	}

	for (auto id = layers.size() - 1; id <= layerID; id++) {
		layers[id] = std::make_shared<LightLayer<FORMAT>>(patternProvider);

		layers[id]->setLayerID(layerID);
		layers[id]->setLightSection(this);
	}
}

LIGHT_SECTION_TEMPLATE
IPixelBuffer *LIGHT_SECTION_CLASS::lockBuffer() {
	// In the case of no buffer pool the output buffer is used.
	// This only works if PIXEL is CRGB, and should otherwise throw an error
	// outputBuffer
	if (bufferPool.size() == 0) return (TPixelBuffer<PIXEL, FORMAT> *)outputBuffer;

	for (uint32_t i = 0; i < bufferPool.size(); i++) {
		uint8_t bit = 1 << i;
		if ((activeBuffers & bit) == 0) {
			activeBuffers |= bit;
			return bufferPool[i];
		}
	}

	return NULL;
}

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::unlockBuffer(IPixelBuffer *buffer) {
	if (buffer == outputBuffer) return;

	for (uint32_t i = 0; i < bufferPool.size(); i++) {
		if (bufferPool[i] == buffer) {
			uint8_t bit = 1 << i;
			activeBuffers &= ~bit;
		}
	}
}

LIGHT_SECTION_TEMPLATE
bool LIGHT_SECTION_CLASS::addBuffer(IPixelBuffer *buffer) {
	if (buffer->getLength() != this->outputBuffer->getLength()) {
#ifdef LS_VERBOSE
#ifdef ARDUINO
		Serial.println(F("ERROR: Buffer added to pool needs to be the same size as the output buffer."));
#else
		fprintf(stderr, "ERROR: Buffer added to pool needs to be the same size as the output buffer.");
#endif
#endif

		return false;
	}

	bufferPool.push_back(buffer);

	return true;
}

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::update() {
	if (bufferPool.size() > 0) {
		outputBuffer->clear();
	}

	for (auto &layer : layers) {
		if (layer == nullptr) continue;
		layer->update();

		auto pattern = layer->getActivePattern();

		if (pattern && !pattern->isFilterPattern()) {
			TPixelBuffer<PIXEL, FORMAT> *buffer = (TPixelBuffer<PIXEL, FORMAT> *)pattern->getPixelBuffer();

			FORMAT opacity = TColorFormatHelper<FORMAT>::scale(layer->getOpacity(), layer->getTransitionOpacity());

			if (opacity < layer->getMaxOpacity()) {
				if (opacity > 0) {
					outputBuffer->blendWith(*buffer, pattern->getBlendMode(), opacity);
				}
			} else {
				outputBuffer->blendWith(*buffer, pattern->getBlendMode());
			}
		}
	}

	updateFadeLevel();

	FORMAT scaledLevel = TColorFormatHelper<FORMAT>::scale(brightness, fade);

	if (scaledLevel < TColorFormatHelper<FORMAT>::getMaxValue()) {
		outputBuffer->fade(scaledLevel);
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
			for (auto &layer : layers) {
				if (!layer) continue;
				layer->pause();
			}
		}
	} else {
		if (!fade) {
			for (auto &layer : layers) {
				if (!layer) continue;
				layer->pause();
			}
		}
	}
}

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::unpause() {
	fadeUp();

	for (auto &layer : layers) {
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
