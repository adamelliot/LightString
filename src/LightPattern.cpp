#include "LightPattern.h"

using namespace LightString;

ILightLayer *ILightPattern::layerAbove() {
	if (layer && layer->getLightSection()) {
		ILightSection *section = layer->getLightSection();
		uint8_t targetLayerID = layer->getLayerID() + 1;

		if (targetLayerID < section->getTotalLayers()) {
			return section->getLayer(targetLayerID);
		}
	}

	return NULL;
}

ILightLayer *ILightPattern::layerBelow() {
	if (layer && layer->getLightSection()) {
		if (layer->getLayerID() == 0) return NULL;

		ILightSection *section = layer->getLightSection();
		uint8_t targetLayerID = layer->getLayerID() - 1;

		return section->getLayer(targetLayerID);
	}

	return NULL;
}

bool ILightPattern::startPatternAbove(PatternCode patternCode) {
	ILightLayer *targetLayer = layerAbove();
	if (targetLayer) {
		return targetLayer->startPattern(patternCode);
	}
	return false;
}

bool ILightPattern::startPatternAbove(uint8_t patternID, uint8_t copyID, uint8_t mode) {
	return startPatternAbove(PatternCode(patternID, copyID, mode));
}

bool ILightPattern::startPatternBelow(PatternCode patternCode) {
	ILightLayer *targetLayer = layerBelow();
	if (targetLayer) {
		return targetLayer->startPattern(patternCode);
	}
	return false;
}

bool ILightPattern::startPatternBelow(uint8_t patternID, uint8_t copyID, uint8_t mode) {
	return startPatternBelow(PatternCode(patternID, copyID, mode));
}

bool ILightPattern::startPatternOnLayer(uint8_t layerID, PatternCode patternCode) {
	if (layer && layer->getLightSection()) {
		ILightSection *section = layer->getLightSection();
		ILightLayer *layer = section->getLayer(layerID);
		
		if (!layer) return false;
		return layer->startPattern(patternCode);
	}

	return false;
}

bool ILightPattern::startPatternOnLayer(uint8_t layerID, uint8_t patternID, uint8_t copyID, uint8_t mode) {
	return startPatternOnLayer(layerID, PatternCode(patternID, copyID, mode));
}

