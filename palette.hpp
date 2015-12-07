
// ------------------- Swatch Manager ---------------------

template<size_t MAX_SWATCHES, typename PIXEL>
void SwatchManager<MAX_SWATCHES, PIXEL>::shuffle() {
	for (size_t i = 0; i < swatchCount; i++) {
		size_t j = i + random() / (0xffffffff / (swatchCount - 1) + 1);
		PIXEL t = swatches[j];
		swatches[j] = swatches[i];
		swatches[i] = t;
	}
}

template<size_t MAX_SWATCHES, typename PIXEL>
void SwatchManager<MAX_SWATCHES, PIXEL>::add(PIXEL color) {
	if (swatchCount >= MAX_SWATCHES) {
		Serial.println("ERROR: Max swatches reached.");
		return;
	}

	swatches[swatchCount++] = color;
}

template<size_t MAX_SWATCHES, typename PIXEL>
TPalette<PIXEL> SwatchManager<MAX_SWATCHES, PIXEL>::generatePalette(uint8_t colorStops, uint8_t padding, bool mirrored) {
	TPalette<PIXEL> ret;

	for (int i = 0; i < padding; i++) {
		ret.colors[i] = PIXEL();
	}

	for (int i = 0; i < colorStops; i++) {
		ret.colors[i + padding] = next();
	}

	if (mirrored) {
		uint8_t half = (padding + colorStops - 1);
		uint8_t total = (half * 2) + 1;
		for (int i = 0; i < half; i++) {
			ret.colors[(total - 1) - i] = ret.colors[i];
		}
		
		ret.size = total;
	} else {
		ret.size = padding + colorStops;
	}

	return ret;
}

template<size_t MAX_SWATCHES, typename PIXEL>
PIXEL SwatchManager<MAX_SWATCHES, PIXEL>::next() {
	swatchIndex++;
	swatchIndex %= swatchCount;

	return swatches[swatchIndex];
}

template<size_t MAX_SWATCHES, typename PIXEL>
PIXEL SwatchManager<MAX_SWATCHES, PIXEL>::previous() {
	if (swatchIndex == 0) {
		swatchIndex = swatchCount - 1;
	} else {
		swatchIndex--;
	}

	return swatches[swatchIndex];
}



// ------------------- Palette Manager ---------------------

template<size_t MAX_PALETTES, typename PIXEL>
void PaletteManager<MAX_PALETTES, PIXEL>::shuffle() {
	for (size_t i = 0; i < paletteCount; i++) {
		size_t j = i + random() / (0xffffffff / (paletteCount - 1) + 1);
		TPalette<PIXEL> *t = palettes[j];
		palettes[j] = palettes[i];
		palettes[i] = t;
	}
}

template<size_t MAX_PALETTES, typename PIXEL>
void PaletteManager<MAX_PALETTES, PIXEL>::add(TPalette<PIXEL> &palette) {
	if (paletteCount >= MAX_PALETTES) {
		Serial.println("ERROR: Max palettes reached.");
		return;
	}

	palettes[paletteCount++] = &palette;
}

template<size_t MAX_PALETTES, typename PIXEL>
void PaletteManager<MAX_PALETTES, PIXEL>::next() {
	paletteIndex++;
	paletteIndex %= paletteCount;
}

template<size_t MAX_PALETTES, typename PIXEL>
void PaletteManager<MAX_PALETTES, PIXEL>::previous() {
	if (paletteIndex == 0) {
		paletteIndex = paletteCount - 1;
	} else {
		paletteIndex--;
	}
}
