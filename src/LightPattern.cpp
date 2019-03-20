#include "LightPattern.h"

using namespace LightString;

ILightLayer *ILightPattern::layerAbove() {
	if (layer && layer->getLightSection()) {
		ILightSection *section = layer->getLightSection();
		auto targetLayerIndex = section->indexOfLayer(layer) + 1;

		return section->getLayer(targetLayerIndex);
	}

	return nullptr;
}

ILightLayer *ILightPattern::layerBelow() {
	if (layer && layer->getLightSection()) {
		ILightSection *section = layer->getLightSection();
		auto targetLayerIndex = section->indexOfLayer(layer) - 1;
		if (targetLayerIndex < 0) return nullptr;

		return section->getLayer(targetLayerIndex);
	}

	return nullptr;
}

bool ILightPattern::startPatternAbove(PatternCode patternCode) {
	ILightLayer *targetLayer = layerAbove();
	if (targetLayer) {
		return targetLayer->startPattern(patternCode);
	}
	return false;
}

bool ILightPattern::startPatternAbove(pattern_id_t patternID, uint8_t mode) {
	return startPatternAbove(PatternCode(patternID, mode));
}

bool ILightPattern::startPatternBelow(PatternCode patternCode) {
	ILightLayer *targetLayer = layerBelow();
	if (targetLayer) {
		return targetLayer->startPattern(patternCode);
	}
	return false;
}

bool ILightPattern::startPatternBelow(pattern_id_t patternID, uint8_t mode) {
	return startPatternBelow(PatternCode(patternID, mode));
}

bool ILightPattern::startPatternOnLayer(int layerIndex, PatternCode patternCode) {
	if (layer && layer->getLightSection()) {
		ILightSection *section = layer->getLightSection();
		if (section->getLayer(layerIndex) == nullptr) return false;

		return section->getLayer(layerIndex)->startPattern(patternCode);
	}

	return false;
}

bool ILightPattern::startPatternOnLayer(int layerIndex, pattern_id_t patternID, uint8_t mode) {
	return startPatternOnLayer(layerIndex, PatternCode(patternID, mode));
}

