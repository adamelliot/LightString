#include "LSLPCustomProgram.h"

void LSLPCustomProgram::setupMode(uint8_t mode) {
	if (this->setupFunc) {
		this->setupFunc(mode);
	}
}

void LSLPCustomProgram::update(uint32_t ms) {
	if (this->updateFunc) {
		this->updateFunc(ms);
	}

	LSLightProgram::update(ms);
}

void LSLPCustomProgram::setProgramID(uint8_t programID) {
	this->programID = programID;
}

void LSLPCustomProgram::setSetupFunc(pcustom_setup_func setupFunc) {
	this->setupFunc = setupFunc;
}

void LSLPCustomProgram::setUpdateFunc(pcustom_update_func updateFunc) {
	this->updateFunc = updateFunc;
}

