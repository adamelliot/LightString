#include "SPI.h"
#include "LSLightString.h"

// === Color Palette ===

color_t colorRGB(uint8_t r, uint8_t g, uint8_t b) {
	return (color_t){r, g, b};
}

color_t colorRBG(uint8_t r, uint8_t g, uint8_t b) {
	return (color_t){r, b, g};
}

color_t colorBGR(uint8_t r, uint8_t g, uint8_t b) {
	return (color_t){b, g, r};
}

color_t randomColor() {
	return (color_t){random(255), random(255), random(255)};
}

color_t whiteColor() {
	return (color_t){255, 255, 255};
}

color_t blackColor() {
	return (color_t){0, 0, 0};
}

LSColorPalette::LSColorPalette() 
 : colorFunc(colorRGB)
{}

void LSColorPalette::setColorFunc(pcolor_func colorFunc) {
	this->colorFunc = colorFunc;
}

pcolor_func LSColorPalette::getColorFunc() {
	return colorFunc;
}

uint8_t LSColorPalette::getFlags() {
	return flags;
}

bool LSColorPalette::isMirrored() {
	return (flags | PALETTE_MIRRORED) == flags;
}

void LSColorPalette::setConfig(void *config) {}
void *LSColorPalette::getConfig() { return NULL; }

color_t LSColorPalette::getColor(uint8_t index) {
	return (color_t){index, index, index};
}

// === Pixel Buffer ===

LSPixelBuffer::LSPixelBuffer(pcolor_t pixels, uint16_t length)
	: pixels(pixels), length(length)
{}

color_t *LSPixelBuffer::getPixels() {
	return pixels;
}

uint16_t LSPixelBuffer::getLength() {
	return length;
}

void LSPixelBuffer::setSize(uint16_t width, uint16_t height) {
	this->width = width;
	this->height = height;
}

uint16_t LSPixelBuffer::getWidth() {
	return this->width;
}

uint16_t LSPixelBuffer::getHeight() {
	return this->height;
}

void LSPixelBuffer::setPixel(uint16_t index, color_t col) {
	pixels[index] = col;
}

void LSPixelBuffer::setMirroredPixel(uint16_t index, color_t col) {
	pixels[index] = col;
	pixels[length - index - 1] = col;
}

color_t LSPixelBuffer::getPixel(uint16_t index) {
	return pixels[index];
}

/**
 * Fast fade doesn't use FP, just shift, only allows 8 steps, but that
 * should be enough most of the time.
 */
void LSPixelBuffer::fade(uint8_t rShift, uint8_t gShift, uint8_t bShift) {
	for (int i = 0; i < length; i++) {
		pixels[i].channels[0] >>= rShift;
		pixels[i].channels[1] >>= gShift;
		pixels[i].channels[2] >>= bShift;
	}
}

void LSPixelBuffer::fade(float ratio) {
	for (int i = 0; i < length; i++) {
		pixels[i].channels[0] *= ratio;
		pixels[i].channels[1] *= ratio;
		pixels[i].channels[2] *= ratio;
	}
}

void LSPixelBuffer::clear() {
	clear((color_t){0, 0, 0});
}

void LSPixelBuffer::clear(color_t col) {
	for (int i = 0; i < length; i++)
		pixels[i] = col;
}

void LSPixelBuffer::shiftUp(uint16_t by) {
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height - 1; y++) {
			pixels[getIndex(x, y)] = pixels[getIndex(x, y + 1)];
		}
	}
}

void LSPixelBuffer::shiftDown(uint16_t by) {
	
}

// TODO: Make this adjustable based on orientation
uint16_t LSPixelBuffer::getIndex(uint8_t x, uint8_t y) {
	return (height * x) + ((x % 2) == 1 ? y : (height - 1) - y);
}

void LSPixelBuffer::setPixel(uint8_t x, uint8_t y, color_t col) {
	pixels[getIndex(x, y)] = col;
}

void LSPixelBuffer::drawColumn(uint8_t x, uint8_t y0, uint8_t y1, color_t col) {
	uint16_t index = getIndex(x, y0);
	int8_t factor = ((x % 2) == 1 ? 1 : -1);

	if (y0 == y1) return;
	if (y0 > y1) {
		uint8_t swap = y0;
		y1 = y0;
		y0 = swap;
	}

	for (int i = 0; i <= y1 - y0; i++)
		pixels[index + i * factor] = col;
}

void LSPixelBuffer::drawRow(uint8_t x0, uint8_t x1, uint8_t y, color_t col) {
	uint16_t index = getIndex(x0, y);
	int8_t swap, step, nextStep;

	if (x0 == x1) return;
	if (x0 > x1) {
		uint8_t swap = x0;
		x1 = x0;
		x0 = swap;
	}

	if ((x0 % 2) == 1) {
		step = ((height - 1) - y) * 2;
		nextStep = y * 2;
	} else {
		step = y * 2;
		nextStep = ((height - 1) - y) * 2;
	}

	for (int i = 0; i <= x1 - x0; i++) {
		pixels[index + i] = col;

		index += step;
		swap = step;
		step = nextStep;
		nextStep = swap;
	}
}

// Ripped from arduino-tvout 
void LSPixelBuffer::drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, color_t col) {
	if (x0 >= width || y0 >= height || x1 >= width || y1 >= height)
		return;

	if (x0 == x1) {
		drawColumn(x0, y0, y1, col);
	} else if (y0 == y1)
		drawRow(x0, x1, y0, col);
	else {
		int e;
		signed int dx, dy,j, temp;
		signed char s1, s2, xchange;
		signed int x, y;

		x = x0;
		y = y0;

		//take absolute value
		if (x1 < x0) {
			dx = x0 - x1;
			s1 = -1;
		}
		else if (x1 == x0) {
			dx = 0;
			s1 = 0;
		}
		else {
			dx = x1 - x0;
			s1 = 1;
		}

		if (y1 < y0) {
			dy = y0 - y1;
			s2 = -1;
		}
		else if (y1 == y0) {
			dy = 0;
			s2 = 0;
		}
		else {
			dy = y1 - y0;
			s2 = 1;
		}

		xchange = 0;	 

		if (dy > dx) {
			temp = dx;
			dx = dy;
			dy = temp;
			xchange = 1;
		} 

		e = ((int)dy << 1) - dx;	

		for (j = 0; j <= dx; j++) {
			setPixel(x, y, col);

			if (e >= 0) {
				if (xchange == 1) x = x + s1;
				else y = y + s2;
				e = e - ((int)dx << 1);
			}
			if (xchange == 1)
				y = y + s2;
			else
				x = x + s1;
			e = e + ((int)dy << 1);
		}
	}
}

// === LED Strip ===
 
// Constructor for use with arbitrary clock/data pins:
LSLEDStrip::LSLEDStrip(uint16_t n, uint8_t dpin, uint8_t cpin) {
	alloc(n);
	updatePins(dpin, cpin);
}

LSLEDStrip::LSLEDStrip(uint16_t n) {
	alloc(n);
	updatePins();
}

// Allocate 3 bytes per pixel, init to RGB 'off' state:
void LSLEDStrip::alloc(uint16_t n) {
	length = n;
	pixels = (pcolor_t)calloc(n, 3);
}

// Change pin assignments post-constructor, using arbitrary pins:
void LSLEDStrip::updatePins(uint8_t dpin, uint8_t cpin) {
	useHardwareSPI = false;

	pinMode(dpin, OUTPUT);
	pinMode(cpin, OUTPUT);

  // Note: any prior clock/data pin directions are left as-is and are
  // NOT restored as inputs!

	useHardwareSPI = false;
	datapin        = dpin;
	clkpin         = cpin;
	clkport        = portOutputRegister(digitalPinToPort(cpin));
	clkpinmask     = digitalPinToBitMask(cpin);
	dataport       = portOutputRegister(digitalPinToPort(dpin));
	datapinmask    = digitalPinToBitMask(dpin);
}

void LSLEDStrip::updatePins(void) {
	useHardwareSPI = true;
	datapin = clkpin = 0;

	// Taken from WS2801 sample code
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(SPI_CLOCK_DIV8);  // 2 MHz
	// WS2801 datasheet recommends max SPI clock of 2 MHz, and 50 Ohm
	// resistors on SPI lines for impedance matching.  In practice and
	// at short distances, 2 MHz seemed to work reliably enough without
	// resistors, and 4 MHz was possible with a 220 Ohm resistor on the
	// SPI clock line only.  Your mileage may vary.  Experiment!
	// SPI.setClockDivider(SPI_CLOCK_DIV4);  // 4 MHz
}

LSPixelBuffer *LSLEDStrip::getPixelBuffer(uint16_t length, uint16_t offset) {
	if (length == 0) length = this->length;

	LSPixelBuffer *ret = new LSPixelBuffer((pixels + offset), length);
	ret->clear();

	return ret;
}

uint16_t LSLEDStrip::getLength(void) {
  return length;
}

void LSLEDStrip::update(void) {
	uint16_t i, nl3 = length * 3;
	uint8_t bit;
	uint8_t *pixels = (uint8_t *)this->pixels;

	if (useHardwareSPI) {
	  for (i=0; i<nl3; i++) {
	    SPDR = pixels[i];
	    while(!(SPSR & (1<<SPIF)));
	  }
	} else {
		for(i = 0; i < nl3; i++) {
			for(bit = 0x80; bit; bit >>= 1) {
				if(pixels[i] & bit) *dataport |=  datapinmask;
				else                *dataport &= ~datapinmask;
				*clkport |=  clkpinmask;
				*clkport &= ~clkpinmask;
			}
		}
	}

	delay(1); // Data is latched by holding clock pin low for 1 millisecond
}

// === Light Program ===

LSLightProgram::LSLightProgram(LSPixelBuffer *pixelBuffer, LSColorPalette *colorPalette, pcolor_func colorFunc)
	: pixelBuffer(pixelBuffer), colorPalette(colorPalette), colorFunc(colorFunc)
{}

LSPixelBuffer *LSLightProgram::getPixelBuffer() {
	return pixelBuffer;
}

void LSLightProgram::setColorFunc(pcolor_func colorFunc) {
	this->colorFunc = colorFunc;
}

pcolor_func LSLightProgram::getColorFunc() {
	return colorFunc;
}

uint8_t LSLightProgram::getFrameRate() {
	return 30;
}

void LSLightProgram::setConfig(void *config) {}
void *LSLightProgram::getConfig() { return NULL; }
void LSLightProgram::update() {}

// === Light Program Manager ===
 
pcolor_palette_t create_color_palette(pcolor_palette_factory_func factory, void *config) {
	pcolor_palette_t ret = (pcolor_palette_t)calloc(sizeof(light_program_t), 1);

	ret->factory = factory;
	ret->config = config;

	return ret;
}

plight_program_t create_light_program(plight_program_factory_func factory, void *config) {
	plight_program_t ret = (plight_program_t)calloc(sizeof(light_program_t), 1);

	ret->factory = factory;
	ret->config = config;

	return ret;
}

plight_section_t create_light_section(pcolor_func colorFunc, LSLEDStrip *lightStrip, uint16_t length, uint16_t offset) {
	plight_section_t ret = (plight_section_t)calloc(sizeof(light_section_t), 1);

	ret->colorFunc = colorFunc;
	ret->pixelBuffer = lightStrip->getPixelBuffer(length, offset);

	return ret;
}

LSLightProgramManager::LSLightProgramManager(uint32_t programLength) 
	: programLength(programLength), activeProgramID(0), programsCount(0),
	lightSectionsCount(0), activeColorPalette(NULL)
{
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

			for (int i = 0; lightStrips[i]; i++)
				lightStrips[i]->update();
			
			delay(20);
		}
	
		for (int i = 0; i < lightSectionsCount; i++) {
			LSPixelBuffer *pixelBuffer = lightSections[i]->activeProgram->getPixelBuffer();
			pixelBuffer->fade(2.6f);
		}

		for (int i = 0; lightStrips[i]; i++)
			lightStrips[i]->update();

		delay(20);
	}
}

void LSLightProgramManager::fadeDown() {
//	pulse();
	
	uint8_t steps = 20;
	uint16_t totalTime = 500;
  uint16_t delayStep = totalTime / steps;

	for (int step = 0; step < steps; step++) {
		float ratio = (float)(steps - (step + 1)) / (float)(steps - step);
		
		for (int i = 0; i < lightSectionsCount; i++) {
			LSPixelBuffer *pixelBuffer = lightSections[i]->activeProgram->getPixelBuffer();
			pixelBuffer->fade(ratio);
		}

		for (int i = 0; lightStrips[i]; i++)
			lightStrips[i]->update();

		delay(delayStep);
	}
	
	delay(200);
}

void LSLightProgramManager::selectNewProgram(uint8_t id) {
	uint8_t lastMode;

//	Serial.println("Selecting program");
	
	// First time through we don't have a palette so don't fade down
	if (activeColorPalette)
		fadeDown();
		
	if (id == MAX_LIGHT_PROGRAMS) {
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

//	Serial.print("activeProgramID: ");
//	Serial.println(activeProgramID);

	uint8_t currentColorPalette = paletteOrder[paletteIndex++];
	paletteIndex %= palettesCount;

	if (activeColorPalette)
		delete activeColorPalette;
	activeColorPalette = colorPalettes[currentColorPalette]->factory();
	if (colorPalettes[currentColorPalette]->config)
		activeColorPalette->setConfig(colorPalettes[currentColorPalette]->config);

//	Serial.print("Color Palette Index: ");
//	Serial.println(currentColorPalette);

	uint8_t newFrameRate = 0;
	void *config = NULL;

	for (int i = 0; i < lightSectionsCount; i++) {
		if (lightSections[i]->supportedPrograms[id]) {
			if (lightSections[i]->activeProgram)
				delete lightSections[i]->activeProgram;
			lightSections[i]->activeProgram = lightSections[i]->supportedPrograms[id]->factory(lightSections[i]->pixelBuffer, activeColorPalette, lightSections[i]->colorFunc);

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

	newFrameRate /= lightSectionsCount;
	msPerFrame = 1000 / newFrameRate;
	lastTime = millis();
//	Serial.println(programLength);
	programSwitchAfter = lastTime + programLength;
}

uint8_t LSLightProgramManager::addLightSection(pcolor_func colorFunc, LSLEDStrip *lightStrip, uint16_t length, uint16_t offset) {
	if (lightSectionsCount >= MAX_LIGHT_SECTIONS) return 0xff;

	if (length == 0) length = lightStrip->getLength();
	lightSections[lightSectionsCount] = create_light_section(colorFunc, lightStrip, length, offset);

	int i = 0;
	do {
		if (lightStrips[i] == lightStrip)
			break;
		if (!lightStrips[i]) {
			lightStrips[i] = lightStrip;
			break;
		}
	} while (i < MAX_LIGHT_SECTIONS && lightStrips[i++]);

	lightSectionsCount++;

	return 1 << (lightSectionsCount - 1);
}

void LSLightProgramManager::addLightProgram(uint8_t strips, uint8_t id, plight_program_factory_func factory, void *config) {
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

void LSLightProgramManager::randomizePaletteOrder() {
	for (int i = 0; i < palettesCount; i++)
		paletteOrder[i] = MAX_COLOR_PALETTES;
	
	uint8_t paletteIndex;
	for (int i = 0; i < palettesCount; i++) {
		while (paletteOrder[paletteIndex = random(palettesCount)] != MAX_COLOR_PALETTES);
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

//	if (timeDelta < msPerFrame)
//		delay(msPerFrame - timeDelta);

	for (int i = 0; i < lightSectionsCount; i++) {
		activeColorPalette->setColorFunc(lightSections[i]->activeProgram->getColorFunc());
		lightSections[i]->activeProgram->update();
	}

	for (int i = 0; lightStrips[i]; i++)
		lightStrips[i]->update();
}

