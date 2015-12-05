// #include "palette.h"

template<size_t MAX_PALETTES, typename PIXEL>
void PaletteManager<MAX_PALETTES, PIXEL>::randomizeOrder() {
	Serial.println(paletteCount);
	
	// TODO: Fix me (I crash)
	
	for (size_t i = 0; i < paletteCount; i++) {
		Serial.println(i);
		size_t j = i + random() / (0xffff / (paletteCount - 1) + 1);
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
	paletteIndex %= getPaletteCount();

	loadPalette(paletteIndex);
}

template<size_t MAX_PALETTES, typename PIXEL>
void PaletteManager<MAX_PALETTES, PIXEL>::previous() {
	if (paletteIndex == 0) {
		paletteIndex = getPaletteCount() - 1;
	} else {
		paletteIndex--;
	}
	
	loadPalette(paletteIndex);
}
