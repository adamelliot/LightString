#include "CustomProgram.h"

void CustomProgram::setupMode(uint8_t mode) {
	if (this->setupFunc) {
		this->setupFunc(mode);
	}
}

void CustomProgram::update(uint32_t ms) {
	if (this->updateFunc) {
		this->updateFunc(ms);
	}

	LightProgram::update(ms);
}

void CustomProgram::setProgramID(uint8_t programID) {
	this->programID = programID;
}

void CustomProgram::setSetupFunc(pcustom_setup_func setupFunc) {
	this->setupFunc = setupFunc;
}

void CustomProgram::setUpdateFunc(pcustom_update_func updateFunc) {
	this->updateFunc = updateFunc;
}

