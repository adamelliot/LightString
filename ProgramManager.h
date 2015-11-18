#ifndef _LIGHTPROGRAMMANAGER_H_
#define _LIGHTPROGRAMMANAGER_H_

#include "FastLED.h"
#include "LightProgram.h"

#define ALL_SECTIONS 0xffff

// #define MAX_MODES 6
// #define MAX_LIGHT_PROGRAMS 6
// #define MAX_LIGHT_SECTIONS 1
// #define MAX_LAYERS 3

#define VERBOSE

using namespace LightString;

namespace LightString {
	
const uint8_t kTransitionFrames = 30;

typedef enum {
	NONE = 0,
	STARTING,
	RUNNING,
	FINISHED
} ETransisionState;

typedef enum {
	PROGRAM_STARTED = 0,
	PROGRAM_FINISHED
} EProgramEvent;

typedef void (* ProgramEvent)(LightProgram &lightProgram, EProgramEvent event);

struct ProgramCode {
	uint8_t programID; // Name of program
	uint8_t copyID; // Which copy of the program
	uint8_t mode; // Which mode is specified

	inline ProgramCode(uint8_t programID = 0, uint8_t copyID = 0, uint8_t mode = 0) __attribute__((always_inline))
		: programID(programID), copyID(copyID), mode(mode) {}

	inline bool operator== (const ProgramCode &rhs) __attribute__((always_inline)) {
		return this->programID == rhs.programID && this->copyID == rhs.copyID && this->mode == rhs.mode;
	}
};

// TODO: Implement program grouping
/*
struct LightProgramGroup {
	uint8_t programID;
	// Pairs of codes and strip addreses
};
*/

template <typename PIXEL, size_t MAX_LIGHT_PROGRAMS>
struct LightLayer {
	uint8_t supportedPrograms[MAX_LIGHT_PROGRAMS];
	uint8_t programCount;
	int16_t programIndexOffset;

	TLightProgram<PIXEL> *activeProgram;

	uint32_t programStartedAt;
	ETransisionState transitionState;

	inline LightLayer()
		: programCount(0), programIndexOffset(0),
		programStartedAt(0), transitionState(NONE) {}
};

template <typename PIXEL, size_t MAX_LIGHT_PROGRAMS, size_t MAX_LAYERS = 1>
struct LightSection {
	CRGBBuffer *outputBuffer;

	// TODO: For transition this should likely be double
	TPixelBuffer<PIXEL> *bufferPool[MAX_LAYERS];
	uint8_t activeBuffers;
	uint8_t bufferCount;

	LightLayer<PIXEL, MAX_LIGHT_PROGRAMS> layers[MAX_LAYERS];

	inline LightSection()
		: outputBuffer(0), activeBuffers(0), totalBuffers(0) {}

	inline LightSection(CRGBBuffer *outputBuffer)
		: outputBuffer(outputBuffer), activeBuffers(0), totalBuffers(0) {}

	TPixelBuffer<PIXEL> *lockBuffer() {
		for (int i = 0; i < bufferCount; i++) {
			uint8_t bit = 1 << i;
			if ((activeBuffers & bit) != 0) {
				activeBuffers |= bit;
				return bufferPool[i];
			}
		}
		
		return NULL;
	}

	void unlockBuffer(TPixelBuffer<PIXEL> *buffer) {
		for (int i = 0; i < bufferCount; i++) {
			if (bufferPool[i] == buffer) {
				uint8_t bit = 1 << i;
				activeBuffers &= ~bit;
			}
		}
	}

	bool addBuffer(TPixelBuffer<PIXEL> *buffer) {
		if (!buffer.length == this->outputBuffer.length) {
#ifdef VERBOSE
			Serial.println(F("ERROR: Buffer added to pool needs to be the same size as the primary buffer."));
#endif

			return false;
		}
		
		if (bufferCount >= (MAX_LAYERS * 2)) return false;
		bufferPool[bufferCount++] = buffer;
	}
};

#define PROGRAM_MANAGER_TEMPLATE template <typename PIXEL, size_t MAX_LAYERS, size_t MAX_MODES, size_t MAX_LIGHT_PROGRAMS, size_t MAX_LIGHT_SECTIONS>
#define PROGRAM_MANAGER_CLASS ProgramManager<PIXEL, MAX_LAYERS, MAX_MODES, MAX_LIGHT_PROGRAMS, MAX_LIGHT_SECTIONS>

// PROGRAM_MANAGER_TEMPLATE
template <typename PIXEL, size_t MAX_LAYERS = 1, size_t MAX_MODES = 6, size_t MAX_LIGHT_PROGRAMS = 6, size_t MAX_LIGHT_SECTIONS = 1>
class ProgramManager {
private:
	LightSection<PIXEL, MAX_LIGHT_PROGRAMS, MAX_LAYERS> lightSections[MAX_LIGHT_SECTIONS];
	uint8_t sectionCount;

	TLightProgram<PIXEL> *lightPrograms[MAX_LIGHT_PROGRAMS];
	ProgramCode programList[MAX_LIGHT_PROGRAMS * MAX_MODES];
	uint8_t programListLength;
	uint8_t programCount;
	uint8_t programOrder[MAX_LIGHT_PROGRAMS * MAX_MODES]; // Order of the program list
	uint8_t programIndex; // Index in the program order

	// Program Manager Timing
	int32_t maxProgramLength; // 0 or less means don't change based on time
	uint32_t lastTime;
	uint16_t msPerFrame;

	bool paused;
	uint32_t pauseStartedAt;

	uint8_t brightness;
	int16_t targetBrightness, adjustedBrightness;
	int16_t brightnessStep;
	
	// Events

	ProgramEvent programEventHandler;

	TLightProgram<PIXEL> *getProgram(ProgramCode programCode);
	TLightProgram<PIXEL> *getProgramForSection(ProgramCode programCode, LightSection<PIXEL, MAX_LIGHT_PROGRAMS, MAX_LAYERS> &section);

	void selectProgramForSection(LightSection<PIXEL, MAX_LIGHT_PROGRAMS, MAX_LAYERS> &section, ProgramCode programCode);
	// void selectProgramGroup(uint8_t programID);
	void nextProgramForSection(LightSection<PIXEL, MAX_LIGHT_PROGRAMS, MAX_LAYERS> &section);
	void normalizeProgramIndices();

	LightSection<PIXEL, MAX_LIGHT_PROGRAMS, MAX_LAYERS> *getLightSection(uint16_t sectionID);

public:

	ProgramManager();

	void setMaxProgramLength(uint32_t maxProgramLength) { this->maxProgramLength = maxProgramLength; }
	int32_t getMaxProgramLength() { return maxProgramLength; }

	void setProgramEventHandler(ProgramEvent programEventHandler) { this->programEventHandler = programEventHandler; }
	ProgramEvent getProgramEventHandler() { return programEventHandler; }

	void setMaxFPS(uint16_t targetFPS) { msPerFrame = 1000 / targetFPS; }
	void pause(bool blackout = true, bool fade = true);
	void unpause();
	void togglePause();

	void nudge(int32_t);

	void selectProgramCode(ProgramCode programCode);

	void selectProgram(uint8_t programID, uint8_t copyID = 0);
	void selectRandomProgram();
	void nextProgram();
	void prevProgram();
	void randomizeProgramOrder();

	void addLightProgram(TLightProgram<PIXEL> &program, uint16_t sections, uint8_t modes[], uint8_t modeCount);
	void addLightProgram(TLightProgram<PIXEL> &program, uint8_t modes[], uint8_t modeCount);
	void addLightProgram(TLightProgram<PIXEL> &program, EProgramMode programModes);
	void addLightProgram(TLightProgram<PIXEL> &program, uint16_t sections);
	void addLightProgram(TLightProgram<PIXEL> &program);

	// void addLightPrograms(LightProgram programs[], uint8_t count);

	uint16_t addLightSection(CRGBBuffer &pixelBuffer);
	bool addBufferToLightSection(uint16_t sectionID, )

	void fadeDown();
	void fadeUp(bool forceZero = true);
	void setBrightness(uint8_t brightness);
	bool isTransitioning();
	void transitionBrightness();

	void finishProgramForSection(LightSection<PIXEL, MAX_LIGHT_PROGRAMS, MAX_LAYERS> &section, uint32_t timeDelta);
	void loop();
};

#include "ProgramManager.tpp"

};

#endif