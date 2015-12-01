#include "LightProgram.h"

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

void ILightProgram::startProgramAbove(ProgramCode programCode) {
	ILightLayer *targetLayer = layerAbove();
	if (targetLayer) {
		targetLayer->startProgram(programCode);
	}
}

void ILightProgram::startProgramAbove(uint8_t programID, uint8_t copyID, uint8_t mode) {
	startProgramAbove(ProgramCode(programID, copyID, mode));
}

void ILightProgram::startProgramBelow(ProgramCode programCode) {
	ILightLayer *targetLayer = layerBelow();
	if (targetLayer) {
		targetLayer->startProgram(programCode);
	}
}

void ILightProgram::startProgramBelow(uint8_t programID, uint8_t copyID, uint8_t mode) {
	startProgramBelow(ProgramCode(programID, copyID, mode));
}
