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

// TODO: Implement program grouping
/*
struct LightProgramGroup {
	uint8_t programID;
	// Pairs of codes and strip addreses
};
*/

#define LIGHT_LAYER_TEMPLATE template <typename PIXEL, size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES>
#define LIGHT_LAYER_CLASS LightLayer<PIXEL, MAX_LIGHT_PROGRAMS, MAX_MODES>

#define LIGHT_SECTION_TEMPLATE template <typename PIXEL, size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES, size_t MAX_LAYERS>
#define LIGHT_SECTION_CLASS LightSection<PIXEL, MAX_LIGHT_PROGRAMS, MAX_MODES, MAX_LAYERS>

#define PROGRAM_MANAGER_TEMPLATE template <typename PIXEL, size_t MAX_LAYERS, size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES, size_t MAX_LIGHT_SECTIONS>
#define PROGRAM_MANAGER_CLASS ProgramManager<PIXEL, MAX_LAYERS, MAX_LIGHT_PROGRAMS, MAX_MODES, MAX_LIGHT_SECTIONS>

LIGHT_SECTION_TEMPLATE
struct LightSection;

LIGHT_LAYER_TEMPLATE
class LightLayer : public ILightLayer {
private:
	uint8_t layerID;

	// TODO: Implement with virtual interface to remove the MAX_LAYER depenance 
	ILightSection *section;

	ILightProgram *lightPrograms[MAX_LIGHT_PROGRAMS];
	uint8_t programCount;

	ProgramCode programList[MAX_LIGHT_PROGRAMS * MAX_MODES];
	uint8_t programListLength;
	uint8_t programIndex; // Index in the program order

	// Program Manager Timing
	// 0 -> let the program choose it's own timing
	// If the program specifies a time and this is set, the progrm will take precident
	uint32_t maxProgramLength;
	uint32_t lastTime;
	uint32_t programStartedAt;
	uint32_t pauseStartedAt;

	ILightProgram *activeProgram;

	ProgramEvent programEventHandler;

	EPlayState playState;
	EPlayMode playMode;
	ETransitionState transitionState;
	
	ILightProgram *getProgram(ProgramCode &programCode);
	void updateProgramIndex(ProgramCode &programCode);
	
public:

	inline LightLayer()
		: programCount(0), programListLength(0), programIndex(0), maxProgramLength(0),
		lastTime(0), programStartedAt(0), pauseStartedAt(0), activeProgram(0),
		programEventHandler(0), playState(PROGRAM_FINISHED), playMode(PLAY_MODE_CONTINUOUS),
		transitionState(TRANSITION_DONE) {}
		
	bool isActive() { return playState != PROGRAM_STOPPED; }

	void setLayerID(uint8_t layerID) { this->layerID = layerID; }
	uint8_t getLayerID() { return layerID; }
	
	void setLightSection(ILightSection *section) { this->section = section; }
	ILightSection *getLightSection() { return section; }

	ILightProgram *getActiveProgram() { return activeProgram; }

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
	
	void addLightProgram(ILightProgram &program, uint64_t modeList);
	void addLightProgram(ILightProgram &program);
	
	void updateTranstion(uint32_t timeDelta);
	void update();
};

LIGHT_SECTION_TEMPLATE
class LightSection : public ILightSection {
private:
	
	IPixelBuffer *bufferPool[MAX_LAYERS];
	uint8_t activeBuffers;
	uint8_t bufferCount;

public:

	CRGBBuffer *outputBuffer;
	LIGHT_LAYER_CLASS layers[MAX_LAYERS];

	inline LightSection()
		: activeBuffers(0), bufferCount(0), outputBuffer(0)
	{
		for (int i = 0; i < MAX_LAYERS; i++) {
			layers[i].setLayerID(i);
			layers[i].setLightSection(this);
		}
	}
	
	CRGBBuffer *getOutputBuffer() { return outputBuffer; }

	uint8_t getMaxLayers() { return MAX_LAYERS; }
	ILightLayer *getLayer(uint8_t layerID) { return &layers[layerID]; }

	IPixelBuffer *lockBuffer();
	void unlockBuffer(IPixelBuffer *buffer);
	bool addBuffer(IPixelBuffer *buffer);

	// Update all the layers and then compact them together into the outputBuffer
	void update();
};

// PROGRAM_MANAGER_TEMPLATE
template <typename PIXEL, size_t MAX_LAYERS = 1, size_t MAX_LIGHT_PROGRAMS = 6, size_t MAX_MODES = 4, size_t MAX_LIGHT_SECTIONS = 1>
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

	void addLightProgram(ILightProgram &program, uint8_t layerID = 0);
	void addLightProgram(ILightProgram &program, uint64_t modeList, uint8_t layerID);
	void addLightProgram(ILightProgram &program, uint64_t modeList, uint8_t layerID, uint8_t sectionID);

	uint8_t addLightSection(CRGBBuffer &pixelBuffer);
	bool addBufferToLightSection(uint8_t sectionID, IPixelBuffer &buffer);

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