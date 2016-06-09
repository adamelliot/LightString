#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/PatternManager.h"

using namespace LightString;

TEST(PatternManager, initialization) {

	PatternManager<TRGB, uint8_t, 1, 1, 1, 1> patternManager;

	// PatternManager is fairly opaque so testing init 
	// means nothing right now

}

TEST(PatternManager, sectionsExist) {
	PatternManager<TRGB, uint8_t, 1, 1, 1, 2> patternManager;
	TPixelBuffer<TRGB, uint8_t> buffer(30);

	uint8_t sectionID = patternManager.addLightSection(buffer);

	auto section = patternManager.getLightSection(sectionID);
	EXPECT_TRUE(section != NULL);

	section = patternManager.getLightSection(1);
	EXPECT_TRUE(section == NULL);
}
