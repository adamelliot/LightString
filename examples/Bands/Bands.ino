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

ProgramManager<Pixel, 3, 3, 1> programManager;

Band<Pixel> band1(Pixel(50, 50, 150, 64));
Band<Pixel> band2(Pixel(CRGB::Orange, 64));
Band<Pixel> band3(Pixel(128, 40, 100, 64));
Band<Pixel> band4(Pixel(0, 128, 192, 64));
Wave wave;

/*
LightProgram programs[] = {
	Band(CRGB::Lime)
};
*/

void setup() {
	Serial.begin(9600);
	delay(500);

	FastLED.addLeds<WS2812B, kDataPin, kColorOrder>(ledBuffer.pixels, kTotalLEDs);
	FastLED.setBrightness(255);

	blink();
	
	uint8_t sectionID = programManager.addLightSection(ledBuffer);
	programManager.addBufferToLightSection(sectionID, backBuffer1);
	programManager.addBufferToLightSection(sectionID, backBuffer2);

	programManager.setMaxProgramLength(kProgramLength);
	programManager.setMaxFPS(30);

	programManager.addLightProgram(band1, 0);
	programManager.addLightProgram(band2, 0);
	programManager.addLightProgram(band3, 1);
	programManager.addLightProgram(band4, 1);
	programManager.addLightProgram(wave, 2);

	blink(CRGB::Lime, 8, 40);

	programManager.startRandomProgram();
}

void loop() {
	// Serial.println("loop");
	programManager.loop();
	FastLED.show();
}