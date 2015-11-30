#ifndef _LIGHTPROGRAMMANAGER_H_
#define _LIGHTPROGRAMMANAGER_H_

#include "FastLED.h"
#include "LightProgram.h"

#define VERBOSE

using namespace LightString;

namespace LightString {
	
const uint8_t kTransitionFrames = 30;

typedef enum {
	TRANSITION_DONE = 0,
	TRANSITION_STARTING,
	TRANSITION_RUNNING
} ETransitionState;

typedef enum {
	PROGRAM_STARTED = 0,
	PROGRAM_PLAYING,
	PROGRAM_FINISHED, // Happens when a program ends
	PROGRAM_PAUSED,
	PROGRAM_STOPPED // Happens after a program finishes, but another isn't started
} EPlayState;

typedef enum {
	PLAY_MODE_CONTINUOUS = 0, // Keep playing another program
	PLAY_MODE_ONCE, // Play the program then stop
	PLAY_MODE_REPEAT // Keep repeating the same program

} EPlayMode;

typedef void (* ProgramEvent)(LightProgram &lightProgram, EPlayState event);

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

#define LIGHT_LAYER_TEMPLATE template <typename PIXEL, size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES, size_t MAX_LAYERS>
#define LIGHT_LAYER_CLASS LightLayer<PIXEL, MAX_LIGHT_PROGRAMS, MAX_MODES, MAX_LAYERS>

#define LIGHT_SECTION_TEMPLATE template <typename PIXEL, size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES, size_t MAX_LAYERS>
#define LIGHT_SECTION_CLASS LightSection<PIXEL, MAX_LIGHT_PROGRAMS, MAX_MODES, MAX_LAYERS>

#define PROGRAM_MANAGER_TEMPLATE template <typename PIXEL, size_t MAX_LAYERS, size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES, size_t MAX_LIGHT_SECTIONS>
#define PROGRAM_MANAGER_CLASS ProgramManager<PIXEL, MAX_LAYERS, MAX_LIGHT_PROGRAMS, MAX_MODES, MAX_LIGHT_SECTIONS>

LIGHT_SECTION_TEMPLATE
struct LightSection;

LIGHT_LAYER_TEMPLATE
struct LightLayer {

	uint8_t layerID;

	// TODO: Implement with virtual interface to remove the MAX_LAYER depenance 
	LIGHT_SECTION_CLASS *section;

	TLightProgram<PIXEL> *lightPrograms[MAX_LIGHT_PROGRAMS];
	uint8_t programCount;

	ProgramCode programList[MAX_LIGHT_PROGRAMS * MAX_MODES];
	uint8_t programListLength;
	uint8_t programOrder[MAX_LIGHT_PROGRAMS * MAX_MODES]; // Order of the program list
	uint8_t programIndex; // Index in the program order

	// Program Manager Timing
	// 0 -> let the program choose it's own timing
	// If the program specifies a time and this is set, the progrm will take precident
	uint32_t maxProgramLength;
	uint32_t lastTime;
	uint32_t programStartedAt;
	uint32_t pauseStartedAt;

	TLightProgram<PIXEL> *activeProgram;

	ProgramEvent programEventHandler;

	EPlayState playState;
	EPlayMode playMode;
	ETransitionState transitionState;
	
private:
	
	TLightProgram<PIXEL> *getProgram(ProgramCode &programCode);
	void updateProgramIndex(ProgramCode &programCode);
	
public:

	inline LightLayer()
		: programCount(0), programListLength(0), programIndex(0), maxProgramLength(0),
		lastTime(0), programStartedAt(0), pauseStartedAt(0), activeProgram(0),
		programEventHandler(0), playState(PROGRAM_FINISHED), playMode(PLAY_MODE_CONTINUOUS),
		transitionState(TRANSITION_DONE) {}
		
	bool isActive() { return playState != PROGRAM_STOPPED; }
	
	void setMaxProgramLength(uint32_t maxProgramLength) { this->maxProgramLength = maxProgramLength; }
	uint32_t getMaxProgramLength() { return maxProgramLength; }

	void setProgramEventHandler(ProgramEvent programEventHandler) { this->programEventHandler = programEventHandler; }
	ProgramEvent getProgramEventHandler() { return programEventHandler; }

	// Play control
	void pause();
	void unpause();

	bool startProgram(ProgramCode &programCode);
	bool startRandomProgram();
	bool nextProgram();
	bool prevProgram();
	
	void randomizeProgramOrder();
	
	void addLightProgram(TLightProgram<PIXEL> &program, uint64_t modeList);
	void addLightProgram(TLightProgram<PIXEL> &program);
	
	void updateTranstion(uint32_t timeDelta);
	void update();
};

LIGHT_SECTION_TEMPLATE
struct LightSection {
	CRGBBuffer *outputBuffer;

	TPixelBuffer<PIXEL> *bufferPool[MAX_LAYERS];
	uint8_t activeBuffers;
	uint8_t bufferCount;

	LIGHT_LAYER_CLASS layers[MAX_LAYERS];

	inline LightSection()
		: outputBuffer(0), activeBuffers(0), bufferCount(0)
	{
		for (int i = 0; i < MAX_LAYERS; i++) {
			layers[i].layerID = i;
			layers[i].section = this;
		}
	}

	TPixelBuffer<PIXEL> *lockBuffer() {
		// In the case of no buffer pool the output buffer is used.
		// This only works if PIXEL is CRGB, and should otherwise throw an error
		// outputBuffer
		if (bufferCount == 0) return (TPixelBuffer<PIXEL> *)outputBuffer;
		
		for (int i = 0; i < bufferCount; i++) {
			uint8_t bit = 1 << i;
			if ((activeBuffers & bit) == 0) {
				activeBuffers |= bit;
				return bufferPool[i];
			}
		}

		return NULL;
	}

	void unlockBuffer(TPixelBuffer<PIXEL> *buffer) {
		if (buffer == (TPixelBuffer<PIXEL> *)outputBuffer) return;

		for (int i = 0; i < bufferCount; i++) {
			if (bufferPool[i] == buffer) {
				uint8_t bit = 1 << i;
				activeBuffers &= ~bit;
			}
		}
	}

	bool addBuffer(TPixelBuffer<PIXEL> *buffer) {
		if (!buffer->length == this->outputBuffer->length) {
#ifdef VERBOSE
			Serial.println(F("ERROR: Buffer added to pool needs to be the same size as the output buffer."));
#endif

			return false;
		}
		
		if (bufferCount >= (MAX_LAYERS * 2)) return false;
		bufferPool[bufferCount++] = buffer;
	}

	// Update all the layers and then compact them together into the outputBuffer
	void update() {
		if (bufferCount > 0) {
			outputBuffer->clear();
		}

		for (int i = 0; i < MAX_LAYERS; i++) {
			layers[i].update();
			outputBuffer->applyCOPY(*layers[i].activeProgram->getPixelBuffer());
		}
	}
};

// PROGRAM_MANAGER_TEMPLATE
template <typename PIXEL, size_t MAX_LAYERS = 1, size_t MAX_LIGHT_PROGRAMS = 6, size_t MAX_MODES = 6, size_t MAX_LIGHT_SECTIONS = 1>
class ProgramManager {
private:
	LIGHT_SECTION_CLASS sections[MAX_LIGHT_SECTIONS];
	uint8_t sectionCount;

	uint32_t lastTime;
	uint16_t msPerFrame;
	/*
	bool paused;
	uint32_t pauseStartedAt;
*/
	uint8_t brightness;
	int16_t targetBrightness, adjustedBrightness;
	int16_t brightnessStep;

	LIGHT_SECTION_CLASS *getLightSection(uint8_t sectionID);

public:

	ProgramManager() : sectionCount(0), msPerFrame(20),
		brightness(255), targetBrightness(255), adjustedBrightness(255),
		brightnessStep(0) {}

	void setMaxProgramLength(uint32_t maxProgramLength);
	void setMaxProgramLength(uint32_t maxProgramLength, uint8_t layerID, uint8_t sectionID = 0);

	void setMaxFPS(uint16_t targetFPS) { msPerFrame = 1000 / targetFPS; }
	void pause(bool blackout = true, bool fade = true);
	void unpause();
	void togglePause();

	// void nudge(int32_t);
	
	bool startProgram(ProgramCode &programCode, uint8_t sectionID, uint8_t layerID);
	bool startProgram(uint8_t programID, uint8_t layerID = 0);
	void startRandomProgram(bool activateLayers = false);
	void startRandomProgram(uint8_t sectionID, uint8_t layerID);

	void nextProgram();
	void nextProgram(uint8_t layerID, uint8_t sectionID = 0);
	void prevProgram();
	void prevProgram(uint8_t layerID, uint8_t sectionID = 0);
	
	void randomizeProgramOrder();

	void addLightProgram(TLightProgram<PIXEL> &program, uint8_t layerID = 0);
	void addLightProgram(TLightProgram<PIXEL> &program, uint64_t modeList, uint8_t layerID);
	void addLightProgram(TLightProgram<PIXEL> &program, uint64_t modeList, uint8_t layerID, uint8_t sectionID);

	uint8_t addLightSection(CRGBBuffer &pixelBuffer);
	bool addBufferToLightSection(uint8_t sectionID, TPixelBuffer<PIXEL> &buffer);

	void fadeDown();
	void fadeUp(bool forceZero = true);
	void setBrightness(uint8_t brightness);
	bool isTransitioning();
	void transitionBrightness();

	void loop();
};

#include "ProgramManager.tpp"

};

#endif