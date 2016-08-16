#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/ThinPatternManager.h"

using namespace LightString;

TEST(ThinPatternManager, initialization) {
	ThinPatternManager<TRGB, uint8_t> patternManager;

	EXPECT_TRUE(patternManager.getOutputBuffer() == NULL);
}

TEST(ThinPatternManager, startRunningUsingRandom) {
	ThinPatternManager<TRGB, uint8_t> patternManager;
	TPixelBuffer<TRGB, uint8_t> leds(5);

	patternManager.setBuffer(&leds);

	TLightPattern<TRGB, uint8_t> testPattern;

	patternManager.addLightPattern(testPattern);
	patternManager.startRandomPattern();

	patternManager.update();
}
