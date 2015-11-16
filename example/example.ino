#include <FastLED.h>
#include <LightString.h>

#include "Band.h"

const uint8_t kDataPin = 11;
const EOrder kColorOrder = GRB;
const uint16_t kTotalLEDs = 144;

// How long each program should run (in ms), set to 0 for unlimitied
const uint32_t kProgramLength = 8000;

// FastLED doesn't like when the pixels are on the stack...
// CRGB leds[kTotalLEDs];
CRGBBuffer ledBuffer(kTotalLEDs);
// PixelBuffer backBuffer1(kTotalLEDs);
// PixelBuffer backBuffer2(kTotalLEDs);

ProgramManager programManager;

Band band1(CRGB::Lime);
Band band2(CRGB::Orange);
Band band3(CRGB(200, 40, 100));

/*
LightProgram programs[] = {
	Band(CRGB::Lime)
};
*/

void setup() {
	Serial.begin(9600);
	delay(100);

	FastLED.addLeds<WS2812B, kDataPin, kColorOrder>(ledBuffer.pixels, kTotalLEDs);
	FastLED.setBrightness(255);

	blink();
	
	programManager.setMaxProgramLength(kProgramLength);
	programManager.setMaxFPS(30);

	programManager.addLightSection(&ledBuffer);
	programManager.addLightProgram(band1);
	programManager.addLightProgram(band2);
	programManager.addLightProgram(band3);

	blink(CRGB::Lime, 8, 40);

	programManager.selectRandomProgram();
}

void loop() {
	programManager.loop();
	FastLED.show();
}