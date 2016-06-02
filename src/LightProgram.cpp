#include "LightProgram.h"

using namespace LightString;

ILightLayer *ILightProgram::layerAbove() {
	if (layer && layer->getLightSection()) {
		ILightSection *section = layer->getLightSection();
		uint8_t targetLayerID = layer->getLayerID() + 1;

		if (targetLayerID < section->getMaxLayers()) {
			return section->getLayer(targetLayerID);
		}
	}

	return NULL;
}

ILightLayer *ILightProgram::layerBelow() {
	if (layer && layer->getLightSection()) {
		if (layer->getLayerID() == 0) return NULL;

		ILightSection *section = layer->getLightSection();
		uint8_t targetLayerID = layer->getLayerID() - 1;

		return section->getLayer(targetLayerID);
	}

	return NULL;
}

bool ILightProgram::startProgramAbove(ProgramCode programCode) {
	ILightLayer *targetLayer = layerAbove();
	if (targetLayer) {
		return targetLayer->startProgram(programCode);
	}
	return false;
}

bool ILightProgram::startProgramAbove(uint8_t programID, uint8_t copyID, uint8_t mode) {
	return startProgramAbove(ProgramCode(programID, copyID, mode));
}

bool ILightProgram::startProgramBelow(ProgramCode programCode) {
	ILightLayer *targetLayer = layerBelow();
	if (targetLayer) {
		return targetLayer->startProgram(programCode);
	}
	return false;
}

bool ILightProgram::startProgramBelow(uint8_t programID, uint8_t copyID, uint8_t mode) {
	return startProgramBelow(ProgramCode(programID, copyID, mode));
}

bool ILightProgram::startProgramOnLayer(uint8_t layerID, ProgramCode programCode) {
	if (layer && layer->getLightSection()) {
		ILightSection *section = layer->getLightSection();
		ILightLayer *layer = section->getLayer(layerID);
		
		if (!layer) return false;
		return layer->startProgram(programCode);
	}

	return false;
}

bool ILightProgram::startProgramOnLayer(uint8_t layerID, uint8_t programID, uint8_t copyID, uint8_t mode) {
	return startProgramOnLayer(layerID, ProgramCode(programID, copyID, mode));
}

