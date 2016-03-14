#include <FastLED.h>
#include <LightString.h>

#include "Band.h"
#include "Wave.h"

const uint8_t kDataPin = 7;
const EOrder kColorOrder = GRB;
const uint16_t kTotalLEDs = 144;

// How long each program should run (in ms), set to 0 for unlimitied
const uint32_t kProgramLength = 8000;

TPixelBuffer<LightString::RGB> ledBuffer(kTotalLEDs);
TPixelBuffer<RGBA> backBuffer1(kTotalLEDs);
TPixelBuffer<RGBA> backBuffer2(kTotalLEDs);

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
	
	////// Tests
	
	RGBu colA0(255, 0, 0);
	RGBu colA1(0, 0, 255);

	colA0 += RGBu(CRGB::Green);
	colA0.println();
	
	RGBAu colB0(0, 255, 255);
	RGBAu colB1(0, 0, 255);

	colA1 += RGBu(CRGB::Green);

	colA0 = colA1;
	colB1 = colA1;
	colA1 = colB1;

	colB0.lerp8(colA0, 127);
	colA1.lerp8(colB1, 127);
	
	colB0.lerp8(CRGB::Blue, 127);
	colB0.println();
	
	colB0 *= 2;
	colB0.println();

	colB0 /= 2;
	colB0.println();
	
	
	///////// END

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