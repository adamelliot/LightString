#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/LightLayer.h"
#include "../src/ThinPatternManager.h"

using namespace LightString;

static int destroyCount = 0;

class TestPattern1 : public TLightPattern<TRGBA, float> {
public:
	TestPattern1() : TLightPattern(3) {}
	~TestPattern1() { destroyCount++; }
	uint8_t getPatternID() { return 1; }
};

class TestPattern2 : public TCloneable<TestPattern2, TLightPattern<TRGBA, float>> {
public:
	TestPattern2() : TCloneable(3) {}
	~TestPattern2() { destroyCount++; }
	uint8_t getPatternID() { return 2; }
};

class TestPattern3 : public TCloneable<TestPattern3, TLightPattern<TRGBA, float>> {
public:
	TestPattern3() : TCloneable(3) {}
	~TestPattern3() { destroyCount++; }
	uint8_t getPatternID() { return 3; }

	EPatternTransition getInTransition() { return TRANSITION_FREEZE_FADE; }
	EPatternTransition getOutTransition() { return TRANSITION_OVERWRITE; }

	int32_t getPatternDuration() { return 3500; }
	int32_t getTransitionDuration() { return 800; }

};

class LightLayerTest
	: public testing::Test
{
protected:

	// Thin pattern manager is a layer with all the "extra" parts need to run
	ThinPatternManager<TRGB, float> lightLayer;
	TPixelBuffer<TRGB, float> leds = TPixelBuffer<TRGB, float>(5);

	TestPattern1 pattern1;
	TestPattern2 pattern2;
	TestPattern3 pattern3;

	virtual void SetUp()
	{
		lightLayer.setBuffer(&leds);

		lightLayer.addLightPattern(pattern1);
		lightLayer.addLightPattern(pattern2);
		lightLayer.addLightPattern(pattern3);
	}

	virtual void TearDown() {
	}
};

TEST(LightLayer, initialization) {
	LightLayer<float> lightLayer;

	EXPECT_EQ(lightLayer.isActive(), false);
	EXPECT_EQ(lightLayer.getOpacity(), 1.0f);
	EXPECT_EQ(lightLayer.getActivePattern(), nullptr);
}

TEST_F(LightLayerTest, creatingPatternSequence) {
	PatternSequence sequence;

	lightLayer.nextPattern();

	sequence.addPatternCue(PatternCode(2, 0, 0), 5000, TRANSITION_OVERWRITE);
	sequence.addPatternCue(PatternCode(1, 0, 0), 5000, TRANSITION_FADE_DOWN);

	lightLayer.setPatternSequence(sequence);

	EXPECT_EQ(lightLayer.getPatternIndex(), 1);

	auto seq = sequence.getSequence();
	EXPECT_EQ(seq.size(), 2);
}

TEST_F(LightLayerTest, playSequence) {
	PatternSequence sequence;

	sequence.addPatternCue(PatternCode(2, 0, 0), 5000, TRANSITION_OVERWRITE);
	sequence.addPatternCue(PatternCode(1, 0, 0), 5000, TRANSITION_FADE_DOWN);

	lightLayer.setPatternSequence(sequence);
	lightLayer.play();

	auto pattern = lightLayer.getActivePattern();

	EXPECT_EQ(pattern->getPatternID(), pattern2.getPatternID());
}

TEST_F(LightLayerTest, nextSequence) {
	PatternSequence sequence;

	sequence.addPatternCue(PatternCode(2, 0, 0), 5000, TRANSITION_OVERWRITE);
	sequence.addPatternCue(PatternCode(1, 0, 0), 5000, TRANSITION_FADE_DOWN);
	sequence.addPatternCue(PatternCode(3, 0, 0), 5000, TRANSITION_FADE_DOWN);

	lightLayer.setPatternSequence(sequence);
	lightLayer.play();

	lightLayer.nextPattern();
	auto pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), pattern1.getPatternID());

	lightLayer.nextPattern();
	pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), pattern3.getPatternID());
}

TEST_F(LightLayerTest, transitionsSelectedFromCorrectPlace) {
	PatternSequence sequence;

	LightLayerConfig config;

	sequence.addPatternCue(PatternCode(2, 0, 0), 5000, TRANSITION_OVERWRITE, TRANSITION_FREEZE_FADE);
	sequence.addPatternCue(PatternCode(1, 0, 0), 5000);
	sequence.addPatternCue(PatternCode(3, 0, 0), 5000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE);

	lightLayer.setPatternSequence(sequence);
	lightLayer.play();

	EXPECT_EQ(lightLayer.getSelectedInTransition(), TRANSITION_OVERWRITE);
	EXPECT_EQ(lightLayer.getSelectedOutTransition(), TRANSITION_FREEZE_FADE);

	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getSelectedInTransition(), config.inTransition);
	EXPECT_EQ(lightLayer.getSelectedOutTransition(), config.outTransition);

	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getSelectedInTransition(), TRANSITION_FREEZE_FADE);
	EXPECT_EQ(lightLayer.getSelectedOutTransition(), TRANSITION_OVERWRITE);
}

TEST_F(LightLayerTest, durationSelectedFromCorrectPlace) {
	PatternSequence sequence;

	LightLayerConfig config;

	config.patternDuration = 10000;

	lightLayer.setConfig(config);

	sequence.addPatternCue(PatternCode(2, 0, 0), 4000, TRANSITION_OVERWRITE, TRANSITION_FREEZE_FADE, 500);
	sequence.addPatternCue(PatternCode(1, 0, 0), -1);
	sequence.addPatternCue(PatternCode(3, 0, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);

	lightLayer.setPatternSequence(sequence);
	lightLayer.play();

	EXPECT_EQ(lightLayer.getSelectedPatternDuration(), 4000);
	EXPECT_EQ(lightLayer.getSelectedInTransitionDuration(), 500);
	EXPECT_EQ(lightLayer.getSelectedOutTransitionDuration(), 500);

	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getSelectedPatternDuration(), 10000);
	EXPECT_EQ(lightLayer.getSelectedInTransitionDuration(), 1000);
	EXPECT_EQ(lightLayer.getSelectedOutTransitionDuration(), 1000);

	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getSelectedPatternDuration(), 3500);
	EXPECT_EQ(lightLayer.getSelectedInTransitionDuration(), 800);
	EXPECT_EQ(lightLayer.getSelectedOutTransitionDuration(), 800);
}

TEST_F(LightLayerTest, patternsClonedOnChange) {
	PatternSequence sequence;

	destroyCount = 0;

	sequence.addPatternCue(PatternCode(2, 0, 0), 4000, TRANSITION_OVERWRITE, TRANSITION_FREEZE_FADE, 500);
	sequence.addPatternCue(PatternCode(1, 0, 0), -1);
	sequence.addPatternCue(PatternCode(3, 0, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);

	lightLayer.setPatternSequence(sequence);
	lightLayer.setClonePatterns(true);

	lightLayer.play();

	auto pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 2);
	EXPECT_NE(pattern, &pattern2);

	lightLayer.nextPattern();
	EXPECT_EQ(destroyCount, 1);

	pattern = lightLayer.getActivePattern();	
	EXPECT_EQ(pattern->getPatternID(), 1);
	EXPECT_EQ(pattern, &pattern1);

	lightLayer.nextPattern();
	EXPECT_EQ(destroyCount, 1);

	pattern = lightLayer.getActivePattern();	
	EXPECT_EQ(pattern->getPatternID(), 3);
	EXPECT_NE(pattern, &pattern3);

	lightLayer.nextPattern();
	EXPECT_EQ(destroyCount, 2);

	pattern = lightLayer.getActivePattern();	
	EXPECT_EQ(pattern->getPatternID(), 2);
}

TEST_F(LightLayerTest, sequencedPatternsLoopPastEndProperly) {
	PatternSequence sequence;

	sequence.addPatternCue(PatternCode(2, 0, 0), 4000, TRANSITION_OVERWRITE, TRANSITION_FREEZE_FADE, 500);
	sequence.addPatternCue(PatternCode(1, 0, 0), -1);
	sequence.addPatternCue(PatternCode(3, 0, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);

	lightLayer.setPatternSequence(sequence);
	lightLayer.setClonePatterns(true);

	lightLayer.play();
	lightLayer.prevPattern();

	auto pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 3);

	lightLayer.nextPattern();

	pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 2);
}

TEST_F(LightLayerTest, patternsLoopPastEndProperly) {
	PatternSequence sequence;

	lightLayer.play();

	auto pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 1);

	lightLayer.prevPattern();

	pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 3);

	lightLayer.nextPattern();

	pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 1);
}

