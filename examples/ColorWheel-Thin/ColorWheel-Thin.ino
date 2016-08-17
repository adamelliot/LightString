#include <FastLED.h>
#define USE_FASTLED
#include <LightString.h>

#include "WheelPattern.h"

const uint8_t kDataPin = 7;
const EOrder kColorOrder = GRB;
const uint16_t kTotalLEDs = 144;

// How long each program should run (in ms), set to 0 for unlimitied
const uint32_t kPatternLength = 8000;

TPixelBuffer<TRGB, uint8_t> ledBuffer(kTotalLEDs);
ThinPatternManager<TRGB, uint8_t> patternManager;

WheelPattern wheelPattern;

void setup() {
	Serial.begin(9600);
	delay(500);

	FastLED.addLeds<WS2812B, kDataPin, kColorOrder>((CRGB *)ledBuffer.pixels, kTotalLEDs);
	FastLED.setBrightness(255);

	blink();

	patternManager.setBuffer(&ledBuffer);
	patternManager.setMaxFPS(30);
	patternManager.setMaxPatternDuration(kPatternLength);
	patternManager.addLightPattern(wheelPattern);

	blink(CRGB::Lime, 8, 40);

	patternManager.startRandomPattern();
}

void loop() {
	if (patternManager.update()) {
		FastLED.show();
	}
}