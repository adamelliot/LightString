#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/ThinPatternManager.h"

using namespace LightString;

class TestPatternProvider : public PatternProvider {
public:
	virtual ILightPattern *patternForID(pattern_id_t patternID, ILightLayer *layer = nullptr) {
		switch (patternID) {
			case 1: return new TSolidColorPattern<TRGB, float>(HTML::Red);
		}

		return nullptr;
	}
};

TEST(ThinPatternManager, initialization) {
	TestPatternProvider provider;
	ThinPatternManager<TRGB, uint8_t> patternManager(provider, 5);

	EXPECT_EQ(patternManager.getOutputBuffer().size(), 5);
}

TEST(ThinPatternManager, startRunningUsingRandom) {
	TestPatternProvider provider;
	ThinPatternManager<TRGB, uint8_t> patternManager(provider, 5);

	patternManager.addLightPattern(1);
	patternManager.startRandomPattern();

	patternManager.update();
}
