#pragma once

#ifndef _LIGHTPROGRAMMANAGER_H_
#define _LIGHTPROGRAMMANAGER_H_

#include "types.h"
#include "LightPattern.h"
#include "LightLayer.h"

using namespace LightString;

namespace LightString {

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
template <template <typename> class PIXEL = TRGB, typename FORMAT = uint8_t, size_t MAX_LAYERS = 1, size_t MAX_LIGHT_PROGRAMS = 6, size_t MAX_MODES = 4, size_t MAX_LIGHT_SECTIONS = 1>
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

public:

	PatternManager() : sectionCount(0), msPerFrame(20),
		brightness(255), targetBrightness(255), adjustedBrightness(255),
		brightnessStep(0) {}

	LIGHT_SECTION_CLASS *getLightSection(uint8_t sectionID);

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