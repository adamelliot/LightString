#include <FastLED.h>
#include <LightString.h>

#include "Band.h"
#include "Wave.h"

const uint8_t kDataPin = 7;
const EOrder kColorOrder = GRB;
const uint16_t kTotalLEDs = 144;

// How long each program should run (in ms), set to 0 for unlimitied
const uint32_t kProgramLength = 8000;

// FastLED doesn't like when the pixels are on the stack...
// CRGB leds[kTotalLEDs];
CRGBBuffer ledBuffer(kTotalLEDs);
PixelBuffer backBuffer1(kTotalLEDs);
PixelBuffer backBuffer2(kTotalLEDs);

ProgramManager<RGBA, 4, 3, 1> programManager;

Band<RGBA> band1(RGBA(50, 50, 150, 64));
Band<RGBA> band2(RGBA(255, 128, 0, 64));
Band<RGBA> band3(RGBA(128, 40, 100, 64));
Band<RGBA> band4(RGBA(0, 192, 50, 64));
Wave wave1, wave2;

void setup() {
	Serial.begin(9600);
	delay(500);

	FastLED.addLeds<WS2812B, kDataPin, kColorOrder>(ledBuffer.pixels, kTotalLEDs);
	FastLED.setBrightness(255);

	blink();
	
	uint8_t sectionID = programManager.addLightSection(ledBuffer);
	programManager.addBufferToLightSection(sectionID, backBuffer1);
	programManager.addBufferToLightSection(sectionID, backBuffer2);

	programManager.setMaxFPS(30);

	programManager.setMaxProgramLength(8000, 0);
	programManager.setMaxProgramLength(5000, 1);

	programManager.addLightProgram(band1, 0);
	programManager.addLightProgram(band2, 0);
	programManager.addLightProgram(band3, 1);
	programManager.addLightProgram(band4, 1);
	programManager.addLightProgram(wave1, 2);
	programManager.addLightProgram(wave2, 3);

	blink(CRGB::Lime, 8, 40);

	programManager.startRandomProgram(0, 0);
	programManager.startRandomProgram(1, 0);
}

void loop() {
	// Serial.println("loop");
	programManager.loop();
	FastLED.show();
}