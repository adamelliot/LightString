#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/LightSection.h"
#include "../src/ThinPatternManager.h"

using namespace LightString;

TEST(LightSection, initialization) {
	LightSection<TRGB, uint8_t> lightSection;

	EXPECT_TRUE(lightSection.outputBuffer == NULL);
}

TEST(LightSection, ensureLayerExists) {
	LightSection<TRGB, uint8_t> lightSection;

	lightSection.ensureLayerExists(4);

	auto layer = lightSection.getLayer(4);

	EXPECT_EQ(layer->getLayerID(), 4);
}

TEST(LightSection, adjustBrightness) {
	LightSection<TRGB, float> lightSection;

	TPixelBuffer<TRGB, float> buffer = TPixelBuffer<TRGB, float>(5);
	TPixelBuffer<TRGB, float> leds = TPixelBuffer<TRGB, float>(5);

	TSolidColorPattern<TRGB, float> pattern(HTML::Red);

	lightSection.outputBuffer = &leds;
	lightSection.addBuffer(&buffer);

	lightSection.ensureLayerExists(0);
	lightSection.layers[0].addLightPattern(pattern);
	lightSection.layers[0].play();

	lightSection.layers[0].getConfig().inTransition = TRANSITION_OVERWRITE;

	lightSection.update();
	EXPECT_RGBf_EQ(leds[0], 1, 0, 0);

	lightSection.setBrightness(0.5f);

	lightSection.update();
	EXPECT_RGBf_EQ(leds[0], 0.5, 0, 0);

	lightSection.setFadeDuration(100);
	lightSection.fadeDown();
	usleep(50000);

	lightSection.update();
	EXPECT_RGBf_EQ(leds[0], 0.25, 0, 0);
}