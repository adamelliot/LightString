#include "LightString.h"
#include "WheelPattern.h"

using namespace LightString;

const uint16_t kTotalLEDs = 100;
const uint32_t kPatternLength = 30000; // 30 seconds

TPixelBuffer<TRGB, float> ledBuffer(kTotalLEDs);
TPixelBuffer<TRGBA, float> backBuffer(kTotalLEDs);

// uint8_t generic_map_plus_one[kTotalLEDs + 1];
// uint8_t *generic_map(generic_map_plus_one + 1);

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

	patternManager.setMaxFPS(60);

	patternManager.addLightSection(ledBuffer);
	patternManager.addBufferToLightSection(0, backBuffer);

	patternManager.setPatternEventHandler(patternEvent);
	patternManager.setMaxPatternLength(kPatternLength);

	patternManager.addLightPattern(wheelPattern);

	patternManager.shufflePatterns();

	patternManager.startRandomPattern(0);
}

void loop() {
	if (patternManager.update()) {
		// Our frame is built and prepped into `ledBuffer` now can be spit out
	}
}
