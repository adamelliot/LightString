// ------------------- Swatch Manager ---------------------

template<template <typename> class T, typename FORMAT>
void TSwatchManager<T, FORMAT>::shuffle() {
	auto size = swatches.size();

	for (size_t i = 0; i < size; i++) {
		size_t j = (i + random() / (0xffffffff / (size - i) + 1)) % size;
		T<FORMAT> t = swatches[j];
		swatches[j] = swatches[i];
		swatches[i] = t;
	}
}

template<template <typename> class T, typename FORMAT>
bool TSwatchManager<T, FORMAT>::add(T<FORMAT> color) {
	swatches.push_back(color);
	return true;
}

template<template <typename> class T, typename FORMAT>
TPalette<T, FORMAT> TSwatchManager<T, FORMAT>::generatePalette(uint8_t colorStops, uint8_t padding, bool mirrored) {
	TPalette<T, FORMAT> ret;

	for (int i = 0; i < padding; i++) {
		ret.colors[i] = T<FORMAT>();
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

template<template <typename> class T, typename FORMAT>
T<FORMAT> TSwatchManager<T, FORMAT>::next() {
	swatchIndex++;
	swatchIndex %= swatches.size();

	return swatches[swatchIndex];
}

template<template <typename> class T, typename FORMAT>
T<FORMAT> TSwatchManager<T, FORMAT>::previous() {
	if (swatchIndex == 0) {
		swatchIndex = swatches.size() - 1;
	} else {
		swatchIndex--;
	}

	return swatches[swatchIndex];
}

// ------------------- Palette Manager ---------------------

template<template <typename> class T, typename FORMAT>
uint8_t TPaletteManager<T, FORMAT>::getPaletteIndexFromID(uint8_t id) {
	for (auto i = 0; i < palettes.size(); i++) {
		if (palettes[i].paletteID == id) {
			return i;
		}
	}

	return 0;
}

template<template <typename> class T, typename FORMAT>
void TPaletteManager<T, FORMAT>::shuffle() {
	auto size = palettes.size();

	for (size_t i = 0; i < size; i++) {
		size_t j = (i + random() / (0xffffffff / (size - i) + 1)) % size;
		TPalette<T, FORMAT> *t = palettes[j];
		palettes[j] = palettes[i];
		palettes[i] = t;
	}
}

template<template <typename> class T, typename FORMAT>
void TPaletteManager<T, FORMAT>::next() {
	paletteIndex++;
	paletteIndex %= palettes.size();
}

template<template <typename> class T, typename FORMAT>
void TPaletteManager<T, FORMAT>::previous() {
	if (paletteIndex == 0) {
		paletteIndex = palettes.size() - 1;
	} else {
		paletteIndex--;
	}
}
