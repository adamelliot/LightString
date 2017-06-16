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
			return new TSolidColorPattern<TRGB, float>(HTML::Red);
		}

		return nullptr;
	}
};

TEST(LightSection, initialization) {
	SectionPatternProvider provider;
	LightSection<TRGB, uint8_t> lightSection(provider);

	EXPECT_TRUE(lightSection.outputBuffer == NULL);
}

TEST(LightSection, ensureLayerExists) {
	SectionPatternProvider provider;
	LightSection<TRGB, uint8_t> lightSection(provider);

	lightSection.ensureLayerExists(4);

	auto layer = lightSection.getLayer(4);

	EXPECT_EQ(layer->getLayerID(), 4);
}

TEST(LightSection, adjustBrightness) {
	SectionPatternProvider provider;
	LightSection<TRGB, float> lightSection(provider);

	TPixelBuffer<TRGB, float> buffer = TPixelBuffer<TRGB, float>(5);
	TPixelBuffer<TRGB, float> leds = TPixelBuffer<TRGB, float>(5);

	lightSection.outputBuffer = &leds;
	lightSection.addBuffer(&buffer);

	lightSection.ensureLayerExists(0);
	lightSection.layers[0]->addLightPattern(1);
	lightSection.layers[0]->play();

	lightSection.layers[0]->getConfig().inTransition = TRANSITION_OVERWRITE;

	lightSection.update();
	EXPECT_RGBf_EQ(leds[0], 1, 0, 0);

	lightSection.setBrightness(0.5f);

	lightSection.update();
	EXPECT_RGBf_EQ(leds[0], 0.5, 0, 0);

	lightSection.setFadeDuration(200);
	lightSection.fadeDown();
	usleep(100000);

	lightSection.update();
	EXPECT_RGBf_EQ(leds[0], 0.25, 0, 0);
}

TEST(LightSection, backBuffersAreProvided) {
	SectionPatternProvider provider;
	LightSection<TRGBA, float, TRGB> lightSection(provider);

	TPixelBuffer<TRGBA, float> buffer = TPixelBuffer<TRGBA, float>(5);
	TPixelBuffer<TRGB, float> leds = TPixelBuffer<TRGB, float>(5);

	lightSection.outputBuffer = &leds;
	lightSection.addBuffer(&buffer);

	auto ret = lightSection.lockBuffer();

	EXPECT_EQ(ret, &buffer);
}
