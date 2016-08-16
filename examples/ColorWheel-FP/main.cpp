#include "LightString.h"
#include "WheelPattern.h"

using namespace LightString;

const uint16_t kTotalLEDs = 100;
const uint32_t kPatternDuration = 10000; // 10 seconds

TPixelBuffer<TRGB, float> ledBuffer(kTotalLEDs);
TPixelBuffer<TRGBA, float> backBuffer(kTotalLEDs);

PatternManager<TRGB, float, 1, 10, 5> patternManager;

// - - - - - - - -  Palettes - - - - - - - - - -

// TPalette<RGBAf> GOLD_LIGHT_BLUE_GRADIENT(RGBAu(0xff, 0xd7, 0), RGBAu(0xAD, 0xD8, 0xE6), RGBAu(0xff, 0xd7, 0));

// - - - - - - - - Light Patterns  - - - - - - - -

WheelPattern wheelPattern;

void setup();
void loop();

int main(int argc, char *argv[]) {
	setup();
	while(true) {
		loop();
	}
}

void patternEvent(ILightPattern &lightPattern, EPlayState state) {
	if (state == PATTERN_FINISHED) {

	}
}

void setup() {
	printf("\n • • • Wheel Startup • • • \n");

	// Sets the target frame rate to run at, patterns may run slower than this, but not faster
	patternManager.setMaxFPS(60);

	// Add a section of lights, this represents a physical section of lights
	patternManager.addLightSection(ledBuffer);

	// Add a buffer that will be operated while a pattern is running
	// You will need to add a buffer for each layer you have in the section
	patternManager.addBufferToLightSection(0, backBuffer);

	// Add callback hook for when pattern events happen
	patternManager.setPatternEventHandler(patternEvent);

	// Set how long our patterns run for by default. This can be over-ridden by the pattern
	patternManager.setMaxPatternDuration(kPatternDuration);

	// Add a light pattern (by default it adds to Section 0, Layer 0)
	patternManager.addLightPattern(wheelPattern);

	// Shuffle the order of patterns, otherwise they play in order they are added
	// and each mode is played per pattern before going to the next pattern.
	// EG: Pattern 0-Mode:0 -> Pattern 0-Mode 1 -> Pattern 1-Mode 0, Pattern 0-Mode 1, etc
	patternManager.shufflePatterns();

	// Start a random program on layer 0
	patternManager.startRandomPattern(0);
}

void loop() {
	// Tell pattern manager to prepare a frame, if this returns false not enough time
	// has passed since the last frame so nothing was done, driving LEDs should only
	// happen when a successful frame is prepared.
	if (patternManager.update()) {
		// Our frame is built and prepped into `ledBuffer` now can be spit out
	}
}
