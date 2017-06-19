#include "LightPattern.h"

using namespace LightString;

ILightLayer *ILightPattern::layerAbove() {
	if (layer && layer->getLightSection()) {
		ILightSection *section = layer->getLightSection();
		uint8_t targetLayerID = layer->getLayerID() + 1;

		if (section->hasLayer(targetLayerID)) {
			return &section->getLayer(targetLayerID);
		}
	}

	return nullptr;
}

ILightLayer *ILightPattern::layerBelow() {
	if (layer && layer->getLightSection()) {
		if (layer->getLayerID() == 0) return NULL;

		ILightSection *section = layer->getLightSection();
		uint8_t targetLayerID = layer->getLayerID() - 1;

		if (section->hasLayer(targetLayerID)) {
			return &section->getLayer(targetLayerID);
		}
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

bool ILightPattern::startPatternOnLayer(uint8_t layerID, PatternCode patternCode) {
	if (layer && layer->getLightSection()) {
		ILightSection *section = layer->getLightSection();
		if (section->hasLayer(layerID)) {
			return section->getLayer(layerID).startPattern(patternCode);
		}
	}

	return false;
}

bool ILightPattern::startPatternOnLayer(uint8_t layerID, pattern_id_t patternID, uint8_t mode) {
	return startPatternOnLayer(layerID, PatternCode(patternID, mode));
}

