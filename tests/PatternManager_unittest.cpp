#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/PatternManager.h"

using namespace LightString;

TEST(PatternManager, initialization) {
	PatternManager<TRGB, uint8_t> patternManager;

	auto section = patternManager.getLightSection(0);

	EXPECT_TRUE(section == NULL);
}

TEST(PatternManager, sectionsExist) {
	PatternManager<TRGB, uint8_t> patternManager;
	TPixelBuffer<TRGB, uint8_t> buffer(30);

	uint8_t sectionID = patternManager.addLightSection(buffer);

	auto section = patternManager.getLightSection(sectionID);
	EXPECT_TRUE(section != NULL);

	section = patternManager.getLightSection(1);
	EXPECT_TRUE(section == NULL);
}

TEST(PatternManager, startRunningUsingPlay) {
	PatternManager<TRGB, uint8_t> patternManager;
	TPixelBuffer<TRGB, uint8_t> leds(30);
	TPixelBuffer<TRGB, uint8_t> buffer(30);

	uint8_t sectionID = patternManager.addLightSection(leds);
	patternManager.addBufferToLightSection(sectionID, buffer);

	auto section = patternManager.getLightSection(sectionID);
	EXPECT_TRUE(section != NULL);

	section = patternManager.getLightSection(1);
	EXPECT_TRUE(section == NULL);

	TLightPattern<TRGB, uint8_t> testPattern;

	patternManager.addLightPattern(testPattern);
	patternManager.play();

	patternManager.update();
}

TEST(PatternManager, startRunningUsingRandom) {
	PatternManager<TRGB, uint8_t> patternManager;
	TPixelBuffer<TRGB, uint8_t> leds(30);
	TPixelBuffer<TRGB, uint8_t> buffer(30);

	uint8_t sectionID = patternManager.addLightSection(leds);
	patternManager.addBufferToLightSection(sectionID, buffer);

	auto section = patternManager.getLightSection(sectionID);
	EXPECT_TRUE(section != NULL);

	section = patternManager.getLightSection(1);
	EXPECT_TRUE(section == NULL);

	TLightPattern<TRGB, uint8_t> testPattern;

	patternManager.addLightPattern(testPattern);
	patternManager.startRandomPattern();

	patternManager.update();
}