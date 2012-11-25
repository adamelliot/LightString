#include "LSLightProgramManager.h"

pcolor_palette_t create_color_palette(pcolor_palette_factory_func factory, void *config) {
	pcolor_palette_t ret = (pcolor_palette_t)calloc(1, sizeof(light_program_t));

	ret->factory = factory;
	ret->config = config;

	return ret;
}

plight_program_t create_light_program(plight_program_factory_func factory, void *config) {
	plight_program_t ret = (plight_program_t)calloc(1, sizeof(light_program_t));

	ret->factory = factory;
	ret->config = config;

	return ret;
}

plight_section_t create_light_section(pcolor_func colorFunc, LSLEDStrip *lightStrip, uint16_t length, uint16_t offset, uint8_t maxLightPrograms) {
	plight_section_t ret = (plight_section_t)calloc(1, sizeof(light_section_t));

	ret->supportedPrograms = (plight_program_t *)calloc(maxLightPrograms, sizeof(plight_program_t));
	ret->colorFunc = colorFunc;
	ret->pixelBuffer = lightStrip->getPixelBuffer(length, offset);

	return ret;
}

void free_light_section(plight_section_t section) {
	free(section->supportedPrograms);
	free(section);
}

LSLightProgramManager::LSLightProgramManager(uint32_t programLength, uint8_t maxLightPrograms, uint8_t maxColorPalettes, uint8_t maxLightSections)
	: programLength(programLength), maxLightPrograms(maxLightPrograms), 
	maxColorPalettes(maxColorPalettes), maxLightSections(maxLightSections),
	activeProgramID(0), programsCount(0), lightSectionsCount(0), activeColorPalette(NULL)
{
	Serial.print(F("\n\n   -=-   Light String Booting   -=-\n\n"));

	programs = (uint8_t *)calloc(maxLightPrograms, sizeof(uint8_t));
	colorPalettes = (pcolor_palette_t *)calloc(maxColorPalettes, sizeof(pcolor_palette_t));
	lightSections = (plight_section_t *)calloc(maxLightSections, sizeof(plight_section_t));
	lightStrips = (LSLEDStrip **)calloc(maxLightSections, sizeof(LSLEDStrip *));
	
	paletteOrder = (uint8_t *)calloc(maxColorPalettes, sizeof(uint8_t));
	programOrder = (uint8_t *)calloc(maxLightPrograms, sizeof(uint8_t));

	loadState();
}

void LSLightProgramManager::saveState() {
	
}

void LSLightProgramManager::loadState() {
	
}

void LSLightProgramManager::pulse() {
	uint8_t pulseDepth = 6;

	for (int k = 0; k < 2; k++) {
		for (int j = 0; j < pulseDepth; j++) {
			for (int i = 0; i < lightSectionsCount; i++) {
				LSPixelBuffer *pixelBuffer = lightSections[i]->activeProgram->getPixelBuffer();
				pixelBuffer->fade(0.88f);
			}

			for (int i = 0; lightStrips[i] && (i < lightSectionsCount); i++)
				(lightStrips[i]->*lightStrips[i]->updateFunc)();
			
			delay(20);
		}
	
		for (int i = 0; i < lightSectionsCount; i++) {
			LSPixelBuffer *pixelBuffer = lightSections[i]->activeProgram->getPixelBuffer();
			pixelBuffer->fade(2.6f);
		}

		for (int i = 0; lightStrips[i] && (i < lightSectionsCount); i++)
			(lightStrips[i]->*lightStrips[i]->updateFunc)();

		delay(20);
	}
}

void LSLightProgramManager::fadeDown() {
//	pulse();
	bool fadeColorPalette = false;
	uint8_t steps = FADE_STEPS;
	uint16_t totalTime = FADE_TIME;
  uint16_t delayStep = totalTime / steps;
	
	for (int i = 0; i < lightSectionsCount; i++) {
		if (lightSections[i]->activeProgram->getPixelBuffer()->useIndexedPixelBuffer()) {
			fadeColorPalette = true;
			break;
		}
	}

	for (int step = 0; step < steps; step++) {
		float ratio = (float)(steps - (step + 1)) / (float)(steps - step);

		activeColorPalette->fade(ratio);

		for (int i = 0; i < lightSectionsCount; i++) {
			LSPixelBuffer *pixelBuffer = lightSections[i]->activeProgram->getPixelBuffer();
			if (!pixelBuffer->useIndexedPixelBuffer())
				pixelBuffer->fade(ratio);
		}

		for (int i = 0; lightStrips[i] && (i < lightSectionsCount); i++)
			(lightStrips[i]->*lightStrips[i]->updateFunc)();

		delay(delayStep);
	}

	delay(200);
}

void LSLightProgramManager::selectNewProgram(uint8_t id) {
	uint8_t lastMode;

	//Serial.println("Selecting program");
	
	// First time through we don't have a palette so don't fade down
	if (activeColorPalette)
		fadeDown();
		
	if (id == ANY_LIGHT_PROGRAM) {
		lastMode = activeProgramID;
		if (programsCount > 1) {
			while(lastMode == activeProgramID) {
				activeProgramID = programs[random(programsCount)];
			}
		}
		else
			activeProgramID = programs[0];
	} else
		activeProgramID = id;

	id = activeProgramID;

	//Serial.print("activeProgramID: ");
	//Serial.println(activeProgramID);

	uint8_t currentColorPalette = paletteOrder[paletteIndex++];
	paletteIndex %= palettesCount;

	if (activeColorPalette)
		delete activeColorPalette;
	activeColorPalette = colorPalettes[currentColorPalette]->factory();
	if (colorPalettes[currentColorPalette]->config)
		activeColorPalette->setConfig(colorPalettes[currentColorPalette]->config);

	//Serial.print("Color Palette Index: ");
	//Serial.println(currentColorPalette);

	uint8_t newFrameRate = 0;
	void *config = NULL;

	for (int i = 0; i < lightSectionsCount; i++) {
		if (lightSections[i]->supportedPrograms[id]) {
			if (lightSections[i]->activeProgram) {
				delete lightSections[i]->activeProgram;
			}
			lightSections[i]->activeProgram = lightSections[i]->supportedPrograms[id]->factory(lightSections[i]->pixelBuffer, activeColorPalette, lightSections[i]->colorFunc);
			lightSections[i]->pixelBuffer->setColorPalette(activeColorPalette);

			if (!config)
				config = lightSections[i]->activeProgram->getConfig();
			
			void *cfg = lightSections[i]->supportedPrograms[id]->config;
			if (!cfg)
				cfg = config;

			if (cfg)
				lightSections[i]->activeProgram->setConfig(cfg);

			newFrameRate += lightSections[i]->activeProgram->getFrameRate();
		}
	}

	for (int i = 0; lightStrips[i] && (i < lightSectionsCount); i++) {
		if (lightStrips[i]->useIndexedPixelBuffer()) {
			// TODO: This is going to be glitchy with divided 
			activeColorPalette->setColorFunc(lightSections[i]->activeProgram->getColorFunc());
			activeColorPalette->generatePaletteTable();
			lightStrips[i]->setColorPalette(activeColorPalette);
		}
	}

	newFrameRate /= lightSectionsCount;
	msPerFrame = 1000 / newFrameRate;
	lastTime = millis();
	//Serial.print("Program Length: ");
	//Serial.println(programLength);
	programSwitchAfter = lastTime + programLength;
}

uint8_t LSLightProgramManager::addLightSection(pcolor_func colorFunc, LSLEDStrip *lightStrip, uint16_t length, uint16_t offset) {
	if (lightSectionsCount >= maxLightSections) {
		Serial.println(F("ERROR: Maximum amount of light sections already added."));
		return 0xff;
	}

	if (length == 0) length = lightStrip->getLength();
	lightSections[lightSectionsCount] = create_light_section(colorFunc, lightStrip, length, offset, maxLightPrograms);

	int i = 0;
	do {
		if (lightStrips[i] == lightStrip)
			break;
		if (!lightStrips[i]) {
			lightStrips[i] = lightStrip;
			break;
		}
	} while (i < maxLightSections && lightStrips[i++]);

	lightSectionsCount++;

	return 1 << (lightSectionsCount - 1);
}

void LSLightProgramManager::addLightProgram(uint8_t strips, uint8_t id, plight_program_factory_func factory, void *config) {
	if (id >= maxLightPrograms) {
		Serial.print(F("ERROR: Light Program not added.\n\tLight program ID ("));
		Serial.print(id);
		Serial.print(F(") is out of range, the maximum ID is: "));
		Serial.println(maxLightPrograms - 1);
		return;
	}
	
	for (int i = 0; i < lightSectionsCount; i++) {
		if (((1 << i) | strips) == strips) {
			lightSections[i]->supportedPrograms[id] = create_light_program(factory, config);
		}
	}

	for (int i = 0; i < programsCount; i++)
		if (programs[i] == id)
			return;

	programs[programsCount++] = id;
}

// TODO: Is this creating a hole for a black palette?
void LSLightProgramManager::randomizePaletteOrder() {
	for (int i = 0; i < palettesCount; i++)
		paletteOrder[i] = maxColorPalettes;
	
	uint8_t paletteIndex;
	for (int i = 0; i < palettesCount; i++) {
		while (paletteOrder[paletteIndex = random(palettesCount)] != maxColorPalettes);
		paletteOrder[paletteIndex] = i;
	}
/*
	for (int i = 0; i < palettesCount; i++) {
		Serial.print(paletteOrder[i], DEC);
		Serial.print(", ");
	}
	
	Serial.println();*/
}

void LSLightProgramManager::addColorPalette(pcolor_palette_factory_func factory, void *config) {
	if (palettesCount >= maxColorPalettes) {
		Serial.println(F("ERROR: Already loaded the maximum amount of palettes."));
		return;
	}

	colorPalettes[palettesCount++] = create_color_palette(factory, config);
	randomizePaletteOrder();
}

void LSLightProgramManager::loop() {
	uint32_t time = millis(), timeDelta = time - lastTime;
	lastTime = time;
	
	if (programSwitchAfter <= time) {
		selectNewProgram();
		return;
	}

	if (timeDelta < msPerFrame)
		delay(msPerFrame - timeDelta);

	for (int i = 0; i < lightSectionsCount; i++) {
		activeColorPalette->setColorFunc(lightSections[i]->activeProgram->getColorFunc());
		lightSections[i]->activeProgram->update();
	}

	for (int i = 0; lightStrips[i] && (i < lightSectionsCount); i++) {
		(lightStrips[i]->*lightStrips[i]->updateFunc)();
	}
}
