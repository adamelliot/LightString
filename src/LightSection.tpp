// ------------------------- Light Section -------------------------

LIGHT_SECTION_TEMPLATE
IPixelBuffer *LIGHT_SECTION_CLASS::lockBuffer() {
	// In the case of no buffer pool the output buffer is used.
	// This only works if PIXEL is CRGB, and should otherwise throw an error
	// outputBuffer

	if (bufferCount == 0) return (TPixelBuffer<PIXEL> *)outputBuffer;
	
	for (uint32_t i = 0; i < bufferCount; i++) {
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
	if (buffer == (IPixelBuffer *)outputBuffer) return;

	for (uint32_t i = 0; i < bufferCount; i++) {
		if (bufferPool[i] == buffer) {
			uint8_t bit = 1 << i;
			activeBuffers &= ~bit;
		}
	}
}

LIGHT_SECTION_TEMPLATE
bool LIGHT_SECTION_CLASS::addBuffer(IPixelBuffer *buffer) {
	if (buffer->getLength() != this->outputBuffer->getLength()) {
#ifdef VERBOSE
#ifdef ARDUINO
		Serial.println(F("ERROR: Buffer added to pool needs to be the same size as the output buffer."));
#else 
		fprintf(stderr, "ERROR: Buffer added to pool needs to be the same size as the output buffer.");
#endif
#endif

		return false;
	}

	if (bufferCount >= (MAX_LAYERS * 2)) return false;
	bufferPool[bufferCount++] = buffer;

	return true;
}

LIGHT_SECTION_TEMPLATE
void LIGHT_SECTION_CLASS::update() {
	if (bufferCount > 0) {
		outputBuffer->clear();
	}

	for (uint32_t i = 0; i < MAX_LAYERS; i++) {
		layers[i].update();
		ILightPattern *pattern = layers[i].getActivePattern();
		
		if (pattern && !pattern->isFilterPattern() && bufferCount > 0) {
			IPixelBuffer *buffer = pattern->getPixelBuffer();
			// TODO: Fix this so it's not destructive and works with FP
			/*
			if (layers[i].getOpacity() < 255) {
				for (uint16_t j = 0; j < buffer->getLength(); j++) {
					// FXIME: This is a hack to get fadeouts to work
					auto alphaBuffer = dynamic_cast<TPixelBuffer<TRGBA, FORMAT> *>(pattern->getPixelBuffer());
					if (alphaBuffer) {
						alphaBuffer->pixels[j].a = scale8(alphaBuffer->pixels[j].a, layers[i].getOpacity());
					}
				}
			}
			*/
			
			// Serial.print("Blend: ");
			// Serial.println(pattern->getBlendMode());
			outputBuffer->blendWith(*buffer, pattern->getBlendMode());
		}
	}
}
