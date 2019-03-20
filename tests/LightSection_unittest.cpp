#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/LightSection.h"
#include "../src/ThinPatternManager.h"

using namespace LightString;

class SectionPatternProvider : public PatternProvider {
public:
	virtual ILightPattern *patternForID(pattern_id_t patternID, ILightLayer *layer = nullptr) {
		switch (patternID) {
		case 1:
			return new TSolidColorPattern<TRGBA, float>(HTML::Red);
		}

		return nullptr;
	}
	virtual ~SectionPatternProvider() {}
};

TEST(LightSection, initialization) {
	SectionPatternProvider provider;
	LightSection<TRGB, uint8_t, TRGBA> lightSection(provider, 5);

	EXPECT_EQ(lightSection.getOutputBuffer().size(), 5);
}

TEST(LightSection, ensureLayerExists) {
	SectionPatternProvider provider;
	LightSection<TRGB, uint8_t, TRGBA> lightSection(provider, 5);

	lightSection.ensureLayerExists(4);

	auto layer = lightSection.getLayer(4);

	EXPECT_EQ(layer->layerIndex(), 4);
}

TEST(LightSection, adjustBrightness) {
	SectionPatternProvider provider;
	LightSection<TRGB, float, TRGBA> lightSection(provider, 5);

	auto & leds = lightSection.getOutputBuffer();

	LightLayerConfig config;
	config.inTransition = TRANSITION_WIPE;
	lightSection.startPatternOnNewLayer(PatternCode(1), config);

	lightSection.update();
	EXPECT_RGBf_EQ(leds[0], 1, 0, 0);
	usleep(5000);

	lightSection.setBrightness(0.5f);

	lightSection.update();
	EXPECT_RGBf_EQ(leds[0], 0.5, 0, 0);

	lightSection.setFadeDuration(200);
	lightSection.fadeDown();
	usleep(100000);
	lightSection.update();

	EXPECT_NEAR(leds[0].r, 0.25, 0.2);
}
