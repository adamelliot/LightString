#pragma once

#ifndef _LIGHTPROGRAMMANAGER_H_
#define _LIGHTPROGRAMMANAGER_H_

#include "LightPattern.h"
#include "utils.h"

using namespace LightString;

namespace LightString {
	
const uint32_t kDefaultTransitionLength = 1000;
const uint8_t kTransitionFrames = 30;

typedef enum {
	TRANSITION_DONE = 0,
	TRANSITION_STARTING,
	TRANSITION_RUNNING
} ETransitionState;

typedef enum {
	PROGRAM_STARTED = 0,
	PROGRAM_PLAYING,
	PROGRAM_FINISHED, // Happens when a pattern ends
	PROGRAM_PAUSED,
	PROGRAM_STOPPED // Happens after a pattern finishes, but another isn't started
} EPlayState;

typedef enum {
	PLAY_MODE_CONTINUOUS = 0, // Keep playing another pattern
	PLAY_MODE_ONCE, // Play the pattern then stop
	PLAY_MODE_REPEAT // Keep repeating the same pattern

} EPlayMode;

typedef void (* PatternEvent)(ILightPattern &lightPattern, EPlayState event);

#define LIGHT_LAYER_TEMPLATE template <size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES>
#define LIGHT_LAYER_CLASS LightLayer<MAX_LIGHT_PROGRAMS, MAX_MODES>

#define LIGHT_SECTION_TEMPLATE template <template <typename> class PIXEL, typename FORMAT, size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES, size_t MAX_LAYERS>
#define LIGHT_SECTION_CLASS LightSection<PIXEL, FORMAT, MAX_LIGHT_PROGRAMS, MAX_MODES, MAX_LAYERS>

#define PROGRAM_MANAGER_TEMPLATE template <template <typename> class PIXEL, typename FORMAT, size_t MAX_LAYERS, size_t MAX_LIGHT_PROGRAMS, size_t MAX_MODES, size_t MAX_LIGHT_SECTIONS>
#define PROGRAM_MANAGER_CLASS PatternManager<PIXEL, FORMAT, MAX_LAYERS, MAX_LIGHT_PROGRAMS, MAX_MODES, MAX_LIGHT_SECTIONS>

LIGHT_SECTION_TEMPLATE
class LightSection;

LIGHT_LAYER_TEMPLATE
class LightLayer : public ILightLayer {
private:
	uint8_t layerID;

	// TODO: Implement with virtual interface to remove the MAX_LAYER dependence 
	ILightSection *section;

	ILightPattern *lightPatterns[MAX_LIGHT_PROGRAMS];
	uint8_t patternCount;

	PatternCode patternList[MAX_LIGHT_PROGRAMS * MAX_MODES];
	uint8_t patternListLength;
	uint8_t patternIndex; // Index in the pattern order

	// Pattern Manager Timing
	// 0 -> let the pattern choose it's own timing
	// If the pattern specifies a time and this is set, the pattern will take precedent
	uint32_t maxPatternLength;
	uint32_t lastTime;
	uint32_t patternStartedAt;
	uint32_t pauseStartedAt;

	uint32_t transitionLength;
	uint32_t transitionStartedAt;
	bool runningBeginTransition;
	
	uint8_t opacity;

	ILightPattern *activePattern;

	PatternEvent patternEventHandler;

	EPlayState playState;
	EPlayMode playMode;
	ETransitionState transitionState;

	ILightPattern *getPattern(PatternCode patternCode);
	void updatePatternIndex(PatternCode patternCode);
	void finishPattern();

	void setPlayState(EPlayState playState);

public:

	inline LightLayer()
		: patternCount(0), patternListLength(0), patternIndex(0), maxPatternLength(0),
		lastTime(0), patternStartedAt(0), pauseStartedAt(0), transitionLength(kDefaultTransitionLength),
		transitionStartedAt(0), opacity(255), activePattern(0),
		patternEventHandler(0), playState(PROGRAM_STOPPED), playMode(PLAY_MODE_CONTINUOUS),
		transitionState(TRANSITION_DONE) {}

	bool isActive() { return playState != PROGRAM_STOPPED; }

	void setLayerID(uint8_t layerID) { this->layerID = layerID; }
	uint8_t getLayerID() { return layerID; }
	
	void setLightSection(ILightSection *section) { this->section = section; }
	ILightSection *getLightSection() { return section; }
	ILightPattern *getActivePattern() { return activePattern; }
	
	inline uint8_t getOpacity() { return opacity; }
	void setPalette(IPalette *palette) { if (activePattern) activePattern->setPalette(palette); }

	void setMaxPatternLength(uint32_t maxPatternLength) { this->maxPatternLength = maxPatternLength; }
	uint32_t getMaxPatternLength() { return maxPatternLength; }

	void setPatternEventHandler(PatternEvent patternEventHandler) { this->patternEventHandler = patternEventHandler; }
	PatternEvent getPatternEventHandler() { return patternEventHandler; }
	
	void setPlayMode(EPlayMode playMode) { this->playMode = playMode; }
	uint32_t getPlayMode() { return playMode; }

	// Play control
	void play(); // Starts from a stopped state, or unpauses.
	void stop();
	void pause();
	void unpause();

	bool startPattern(PatternCode patternCode);
	bool startRandomPattern();
	bool nextPattern();
	bool prevPattern();

	void shufflePatterns();

	void addLightPattern(ILightPattern &pattern, uint64_t modeList);
	void addLightPattern(ILightPattern &pattern);
	
	void updateTransition(uint32_t timeDelta);
	void update();
};

LIGHT_SECTION_TEMPLATE
class LightSection : public ILightSection {
private:
	
	IPixelBuffer *bufferPool[MAX_LAYERS];
	uint8_t activeBuffers;
	uint8_t bufferCount;

public:

	TPixelBuffer<PIXEL, FORMAT> *outputBuffer;
	LIGHT_LAYER_CLASS layers[MAX_LAYERS];

	inline LightSection()
		: activeBuffers(0), bufferCount(0), outputBuffer(0)
	{
		for (int i = 0; i < MAX_LAYERS; i++) {
			layers[i].setLayerID(i);
			layers[i].setLightSection(this);
		}
	}

	TPixelBuffer<PIXEL, FORMAT> *getOutputBuffer() { return outputBuffer; }

	uint8_t getMaxLayers() { return MAX_LAYERS; }
	ILightLayer *getLayer(uint8_t layerID) { return &layers[layerID]; }

	IPixelBuffer *lockBuffer();
	void unlockBuffer(IPixelBuffer *buffer);
	bool addBuffer(IPixelBuffer *buffer);

	// Update all the layers and then compact them together into the outputBuffer
	void update();
};

// PROGRAM_MANAGER_TEMPLATE
template <template <typename> class PIXEL, typename FORMAT, size_t MAX_LAYERS = 1, size_t MAX_LIGHT_PROGRAMS = 6, size_t MAX_MODES = 4, size_t MAX_LIGHT_SECTIONS = 1>
class PatternManager {
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

	PatternManager() : sectionCount(0), msPerFrame(20),
		brightness(255), targetBrightness(255), adjustedBrightness(255),
		brightnessStep(0) {}

	void setPatternEventHandler(PatternEvent patternEventHandler);
	void setPatternEventHandler(PatternEvent patternEventHandler, uint8_t layerID, uint8_t sectionID = 0);

	void setMaxPatternLength(uint32_t maxPatternLength);
	void setMaxPatternLength(uint32_t maxPatternLength, uint8_t layerID, uint8_t sectionID = 0);
	
	void setPlayMode(EPlayMode playMode);
	void setPlayMode(EPlayMode playMode, uint8_t layerID, uint8_t sectionID = 0);

	void setMaxFPS(uint16_t targetFPS) { 
		msPerFrame = (targetFPS == 0) ? 0 : 1000 / targetFPS; 
	}
	
	void pause(bool blackout = true, bool fade = true);
	void unpause();

	bool startPattern(PatternCode patternCode, uint8_t layerID, uint8_t sectionID = 0);
	bool startPattern(uint8_t patternID, uint8_t layerID = 0);

	// Only starts on active layers
	void startRandomPattern();
	void startRandomPatternOnAllLayers();
	void startRandomPattern(uint8_t layerID, uint8_t sectionID = 0);

	// Start the current pattern
	void play();
	void play(uint8_t layerID, uint8_t sectionID = 0);

	void nextPattern();
	void nextPattern(uint8_t layerID, uint8_t sectionID = 0);
	void prevPattern();
	void prevPattern(uint8_t layerID, uint8_t sectionID = 0);

	void shufflePatterns();

	void addLightPattern(ILightPattern &pattern, uint8_t layerID = 0);
	void addLightPattern(ILightPattern &pattern, uint64_t modeList, uint8_t layerID);
	void addLightPattern(ILightPattern &pattern, uint64_t modeList, uint8_t layerID, uint8_t sectionID);

	uint8_t addLightSection(TPixelBuffer<PIXEL, FORMAT> &pixelBuffer);
	bool addBufferToLightSection(uint8_t sectionID, IPixelBuffer &buffer);

	void fadeDown();
	void fadeUp(bool forceZero = true);
	void setBrightness(uint8_t brightness);
	bool isTransitioning();
	void transitionBrightness();

	bool loop();
};

#include "PatternManager.tpp"

};

#endif