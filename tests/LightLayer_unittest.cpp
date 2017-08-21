#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/LightLayer.h"
#include "../src/ThinPatternManager.h"

using namespace LightString;

static int destroyCount = 0;
static int setup1Count = 0;

class TestPattern1 : public TLightPattern<TRGBA, float, 4> {
public:
	TestPattern1() {}
	~TestPattern1() { destroyCount++; }
	void setupMode(uint8_t, PatternConfig *) { setup1Count++; }
};

class TestPattern2 : public TLightPattern<TRGBA, float, 3> {
public:
	TestPattern2() {}
	~TestPattern2() { destroyCount++; }
};

template <unsigned MODE_COUNT>
class BasePattern : public TLightPattern<TRGBA, float, MODE_COUNT> {
public:
	BasePattern() {}
};

class TestPattern3 : public BasePattern<5> {
public:
	TestPattern3() {}
	~TestPattern3() { destroyCount++; }

	EPatternTransition getInTransition() { return TRANSITION_FREEZE_FADE; }
	EPatternTransition getOutTransition() { return TRANSITION_OVERWRITE; }

	int32_t getPatternDuration() { return 3500; }
	int32_t getTransitionDuration() { return 800; }

};

class TestPattern4 : public BasePattern<5> {
public:
	TestPattern4() {}
	~TestPattern4() { destroyCount++; }

	EPatternTransition getInTransition() { return TRANSITION_FREEZE_FADE; }
	EPatternTransition getOutTransition() { return TRANSITION_OVERWRITE; }

};

class TestPattern5 : public BasePattern<5> {
public:
	TestPattern5() {}
	~TestPattern5() { destroyCount++; }

	EPatternTransition getInTransition() { return TRANSITION_FREEZE_FADE; }
	EPatternTransition getOutTransition() { return TRANSITION_OVERWRITE; }

};

class TestPatternProvider : public PatternProvider {
public:

	virtual ILightPattern *patternForID(pattern_id_t patternID, ILightLayer *layer = nullptr) {
		switch (patternID) {
			case 1: return new TestPattern1;
			case 2: return new TestPattern2;
			case 3: return new TestPattern3;
			case 4: return new TestPattern4;
			case 5: return new TestPattern5;
		}

		return nullptr;
	}
};

class LightLayerTest
	: public testing::Test
{
protected:

	// Thin pattern manager is a layer with all the "extra" parts need to run
	TestPatternProvider provider;
	ThinPatternManager<TRGB, float> lightLayer = ThinPatternManager<TRGB, float>(provider);
	TPixelBuffer<TRGB, float> leds = TPixelBuffer<TRGB, float>(5);

	void runLayerFor(int ms, int msPerFrame = 25) {
		auto lastTime = millis();
		auto endTime = ms + lastTime;
		lightLayer.update();

		auto now = millis();

		while (endTime > now) {
			auto delay = msPerFrame - (now - lastTime);
			usleep(delay * 1000);

			now = millis();
			lightLayer.update();
			lastTime = now;
		}
	}

	virtual void SetUp()
	{
		lightLayer.setBuffer(&leds);
	}

	virtual void TearDown() {
	}
};

TEST(LightLayer, initialization) {
	TestPatternProvider provider;
	LightLayer<float> lightLayer(provider);

	EXPECT_EQ(lightLayer.isActive(), false);
	EXPECT_EQ(lightLayer.getOpacity(), 1.0f);
	EXPECT_EQ(lightLayer.getActivePattern(), nullptr);
}

TEST(LightLayer, startPatternWithNoBuffersFailsGracefully) {
	TestPatternProvider provider;
	LightLayer<float> lightLayer(provider);

	lightLayer.startPattern(1);

	EXPECT_EQ(lightLayer.getActivePattern(), nullptr);
	EXPECT_EQ(lightLayer.getPlayState(), PATTERN_STOPPED);
}

/* ---------- PLAYBACK TESTS ----------- */

TEST_F(LightLayerTest, enqueuePattern) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.enqueuePattern(PatternCode(5));
	runLayerFor(150);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	runLayerFor(200);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 5);
}

TEST_F(LightLayerTest, enqueuePatternAndWait) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.enqueuePattern(PatternCode(5), true);
	runLayerFor(150);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	runLayerFor(350);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 5);

	EXPECT_FALSE(lightLayer.isRunningPatternFromSequence());
}

TEST_F(LightLayerTest, startPattern) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	lightLayer.startPattern(PatternCode(5));
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 5);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 5);

	EXPECT_FALSE(lightLayer.isRunningPatternFromSequence());
}

TEST_F(LightLayerTest, startPatternWithTransition) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	lightLayer.startPattern(PatternCode(5), true);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 5);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 5);

	EXPECT_FALSE(lightLayer.isRunningPatternFromSequence());
}

TEST_F(LightLayerTest, startPatternWithTransitionWhilePaused) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
	lightLayer.pause();

	lightLayer.startPattern(PatternCode(5), true);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);
	runLayerFor(500);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 5);

	EXPECT_FALSE(lightLayer.isRunningPatternFromSequence());
}

TEST_F(LightLayerTest, startPatternWhileSequenceIsPlaying) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.startPattern(PatternCode(5));
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 5);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 5);

	EXPECT_FALSE(lightLayer.isRunningPatternFromSequence());
}

TEST_F(LightLayerTest, enqueuePatternAtIndex) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.enqueuePatternAtIndex(2);
	runLayerFor(150);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	runLayerFor(200);
	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
}

TEST_F(LightLayerTest, enqueuePatternAtIndexFailsWithBadIndex) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	EXPECT_FALSE(lightLayer.enqueuePatternAtIndex(10));
	runLayerFor(150);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
}

TEST_F(LightLayerTest, enqueuePatternAtIndexFailsWithNoSequence) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	lightLayer.play();
	runLayerFor(50);
	EXPECT_FALSE(lightLayer.isActive());

	EXPECT_FALSE(lightLayer.enqueuePatternAtIndex(10));
	EXPECT_FALSE(lightLayer.isActive());
}

TEST_F(LightLayerTest, enqueuePatternAtIndexAndWait) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.enqueuePatternAtIndex(2, true);
	runLayerFor(150);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	runLayerFor(200);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	runLayerFor(150);
	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
}

TEST_F(LightLayerTest, startPatternAtIndex) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.startPatternAtIndex(2);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
}

TEST_F(LightLayerTest, startPatternAtIndexFailsWithBadIndex) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	EXPECT_FALSE(lightLayer.startPatternAtIndex(10));
	runLayerFor(150);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
}

TEST_F(LightLayerTest, startPatternAtIndexFailsWithNoSequence) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	lightLayer.play();
	runLayerFor(50);
	EXPECT_FALSE(lightLayer.isActive());

	EXPECT_FALSE(lightLayer.startPatternAtIndex(10));
	EXPECT_FALSE(lightLayer.isActive());
}

TEST_F(LightLayerTest, nextPatternWithNoSequenceAndEnqueuedPattern) {
	lightLayer.getConfig().patternDuration = 500;
	lightLayer.getConfig().inTransitionDuration = 100;
	lightLayer.getConfig().outTransitionDuration = 100;

	lightLayer.startPattern(PatternCode(1));
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 1);

	lightLayer.startPattern(PatternCode(2));
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);

	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);

	lightLayer.enqueuePattern(PatternCode(4));
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);
	runLayerFor(500);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 4);
	runLayerFor(500);

	EXPECT_FALSE(lightLayer.isActive());
}

TEST_F(LightLayerTest, nextPattern) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);

	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 2);

	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
}

TEST_F(LightLayerTest, nextPatternWithTransition) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(200);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.nextPattern(true);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
	runLayerFor(250);
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);

	lightLayer.nextPattern(true);
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);
	runLayerFor(500);
	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
}

TEST_F(LightLayerTest, startPatternAtIndexWhilePaused) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	lightLayer.setFadeDuration(100);

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(5, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);

	lightLayer.pause(false, false);
	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 1);
	EXPECT_TRUE(lightLayer.willStop());

	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 3);

	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 1);

	lightLayer.startPatternAtIndex(1);
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);
	EXPECT_EQ(lightLayer.isPaused(), false);

	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 1);
	runLayerFor(550, 20);
	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
}

TEST_F(LightLayerTest, nextPatternWhilePaused) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	lightLayer.setFadeDuration(100);

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(5, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);

	lightLayer.pause(false, false);
	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 1);
	EXPECT_TRUE(lightLayer.willStop());

	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 3);

	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 1);

	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
	lightLayer.unpause();
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 1);
	runLayerFor(100, 20);
	EXPECT_EQ(lightLayer.getActivePattern(), nullptr);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.play();
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
}

TEST_F(LightLayerTest, nextPatternWhileStopped) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(5, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);

	lightLayer.stop(false);
	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getActivePattern(), nullptr);

	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 3);

	EXPECT_EQ(lightLayer.getActivePattern(), nullptr);

	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.play();
	runLayerFor(50, 20);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);
}

TEST_F(LightLayerTest, nextPatternWhileStoppedDuringFadeout) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(5, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);

	lightLayer.stop(true);
	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 1);

	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 3);

	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 1);
	runLayerFor(200);
	EXPECT_EQ(lightLayer.getActivePattern(), nullptr);

	lightLayer.nextPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);
}

TEST_F(LightLayerTest, prevPattern) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.prevPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 2);

	lightLayer.prevPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);

	lightLayer.prevPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
}

TEST_F(LightLayerTest, prevPatternWithTransition) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(200);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.prevPattern(true);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
	runLayerFor(250);
	EXPECT_EQ(lightLayer.getPatternIndex(), 2);

	lightLayer.prevPattern(true);
	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
	runLayerFor(500);
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);
}

TEST_F(LightLayerTest, prevPatternWhilePaused) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	lightLayer.setFadeDuration(100);

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(5, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.prevPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 3);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 3);

	lightLayer.pause(false, false);
	lightLayer.prevPattern();
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 5);
	EXPECT_TRUE(lightLayer.willStop());

	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
	lightLayer.prevPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);

	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 5);

	lightLayer.prevPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
	lightLayer.unpause();
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	EXPECT_EQ(lightLayer.getActivePattern(), nullptr);

	lightLayer.play();
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
}

TEST_F(LightLayerTest, prevPatternWhileStopped) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(5, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.prevPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 3);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 3);

	lightLayer.stop(false);
	lightLayer.prevPattern();
	EXPECT_EQ(lightLayer.getActivePattern(), nullptr);

	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
	lightLayer.prevPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);

	EXPECT_EQ(lightLayer.getActivePattern(), nullptr);

	lightLayer.prevPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.play();
	runLayerFor(50, 20);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);
}

TEST_F(LightLayerTest, prevPatternWhileStoppedDuringFadeout) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(5, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.prevPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 3);
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 3);

	lightLayer.stop(true);
	lightLayer.prevPattern();
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 5);

	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
	lightLayer.prevPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);

	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 5);
	runLayerFor(200);
	EXPECT_EQ(lightLayer.getActivePattern(), nullptr);

	lightLayer.prevPattern();
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	lightLayer.play();
	runLayerFor(60, 20);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);

	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);
}

TEST_F(LightLayerTest, stop) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	lightLayer.nextPattern();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);

	lightLayer.stop(false);
	EXPECT_FALSE(lightLayer.isActive());
	runLayerFor(100);
	EXPECT_FALSE(lightLayer.isActive());
}

TEST_F(LightLayerTest, stopWithFadeOut) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 500;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 200);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 200);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	lightLayer.nextPattern();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);

	lightLayer.stop();
	EXPECT_TRUE(lightLayer.isActive());
	runLayerFor(200);
	EXPECT_TRUE(lightLayer.isActive());
	runLayerFor(400);
	EXPECT_FALSE(lightLayer.isActive());
}

TEST_F(LightLayerTest, patternDoesntAdvanceWhenHeld) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 400;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 100);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 100);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 100);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);
	lightLayer.hold();
	runLayerFor(500);

	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);

	lightLayer.nextPattern();

	EXPECT_EQ(lightLayer.getPatternIndex(), 1);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 1);

	runLayerFor(500);
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 1);
}

TEST_F(LightLayerTest, patternAdvancesAfterRemovingHold) {
	PatternSequence sequence;

	lightLayer.getConfig().patternDuration = 400;

	sequence.addPatternCue(PatternCode(2, 0), -1, TRANSITION_FADE_UP, TRANSITION_OVERWRITE, 50);
	sequence.addPatternCue(PatternCode(1, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 50);
	sequence.addPatternCue(PatternCode(4, 0), -1, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 50);

	lightLayer.setPatternSequence(sequence);

	lightLayer.play();
	runLayerFor(100);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);
	lightLayer.hold();
	runLayerFor(500);

	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);

	lightLayer.unhold();

	runLayerFor(100);

	EXPECT_EQ(lightLayer.getPatternIndex(), 1);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 1);

	runLayerFor(500);
	EXPECT_EQ(lightLayer.getPatternIndex(), 2);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 4);
}

/* -------------  SEQUENCE TESTS ---------------- */

TEST_F(LightLayerTest, addingPatternsDirectlyUsesSimpleSequence) {
	lightLayer.getConfig().patternDuration = 500;
	lightLayer.getConfig().inTransitionDuration = 100;
	lightLayer.getConfig().outTransitionDuration = 100;

	lightLayer.addLightPattern(1);

	lightLayer.play();

	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
	runLayerFor(50, 20);
	EXPECT_EQ(lightLayer.getPlayState(), PATTERN_PLAYING_IN_TRANSITION);

	runLayerFor(300, 20);
	EXPECT_EQ(lightLayer.getPlayState(), PATTERN_PLAYING);

	runLayerFor(100, 20);
	EXPECT_EQ(lightLayer.getPlayState(), PATTERN_PLAYING_OUT_TRANSITION);

	runLayerFor(500, 20);
	EXPECT_EQ(lightLayer.getPatternIndex(), 1);

	runLayerFor(500, 20);
	EXPECT_EQ(lightLayer.getPatternIndex(), 2);

	runLayerFor(500, 20);
	EXPECT_EQ(lightLayer.getPatternIndex(), 3);

	runLayerFor(500, 20);
	EXPECT_EQ(lightLayer.getPatternIndex(), 0);
}

TEST_F(LightLayerTest, creatingPatternSequence) {
	PatternSequence sequence;

	lightLayer.nextPattern();

	sequence.addPatternCue(PatternCode(2, 0), 5000, TRANSITION_OVERWRITE);
	sequence.addPatternCue(PatternCode(1, 0), 5000, TRANSITION_FADE_DOWN);

	lightLayer.setPatternSequence(sequence, 1);

	EXPECT_EQ(lightLayer.getPatternIndex(), 1);

	auto seq = sequence.getSequence();
	EXPECT_EQ(seq.size(), 2);
}

TEST_F(LightLayerTest, playSequence) {
	PatternSequence sequence;

	sequence.addPatternCue(PatternCode(2, 0), 5000, TRANSITION_OVERWRITE);
	sequence.addPatternCue(PatternCode(1, 0), 5000, TRANSITION_FADE_DOWN);

	lightLayer.setPatternSequence(sequence);
	lightLayer.play();

	auto pattern = lightLayer.getActivePattern();

	EXPECT_EQ(pattern->getPatternID(), 2);
}

TEST_F(LightLayerTest, nextSequence) {
	PatternSequence sequence;

	sequence.addPatternCue(PatternCode(2, 0), 5000, TRANSITION_OVERWRITE);
	sequence.addPatternCue(PatternCode(1, 0), 5000, TRANSITION_FADE_DOWN);
	sequence.addPatternCue(PatternCode(3, 0), 5000, TRANSITION_FADE_DOWN);

	lightLayer.setPatternSequence(sequence);
	lightLayer.play();

	lightLayer.nextPattern();
	auto pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 1);

	lightLayer.nextPattern();
	pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 3);
}

TEST_F(LightLayerTest, transitionsSelectedFromCorrectPlace) {
	PatternSequence sequence;

	LightLayerConfig config;

	sequence.addPatternCue(PatternCode(2, 0), 5000, TRANSITION_OVERWRITE, TRANSITION_FREEZE_FADE);
	sequence.addPatternCue(PatternCode(1, 0), 5000);
	sequence.addPatternCue(PatternCode(3, 0), 5000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE);

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

	sequence.addPatternCue(PatternCode(2, 0), 4000, TRANSITION_OVERWRITE, TRANSITION_FREEZE_FADE, 500);
	sequence.addPatternCue(PatternCode(1, 0), -1);
	sequence.addPatternCue(PatternCode(3, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);

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

TEST_F(LightLayerTest, sequencedPatternsLoopPastEndProperly) {
	PatternSequence sequence;

	sequence.addPatternCue(PatternCode(2, 0), 4000, TRANSITION_OVERWRITE, TRANSITION_FREEZE_FADE, 500);
	sequence.addPatternCue(PatternCode(1, 0), -1);
	sequence.addPatternCue(PatternCode(3, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);

	lightLayer.setPatternSequence(sequence);

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

	lightLayer.addLightPattern(1);
	lightLayer.addLightPattern(2);
	lightLayer.addLightPattern(3);

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

TEST_F(LightLayerTest, playStatesProperlyChange) {
	PatternSequence sequence;

	sequence.addPatternCue(PatternCode(2, 0), 500, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 50);
	sequence.addPatternCue(PatternCode(1, 0), 500, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 50);
	sequence.addPatternCue(PatternCode(3, 0), 500, TRANSITION_FADE_UP, TRANSITION_FADE_DOWN, 50);

	lightLayer.setPatternSequence(sequence, 0);

	lightLayer.update();
	EXPECT_EQ(lightLayer.getPlayState(), PATTERN_PLAYING_IN_TRANSITION);
	usleep(55000);

	lightLayer.update();
	EXPECT_EQ(lightLayer.getPlayState(), PATTERN_PLAYING);
	usleep(500000);

	lightLayer.update();
	EXPECT_EQ(lightLayer.getPlayState(), PATTERN_PLAYING_OUT_TRANSITION);
	usleep(50000);

	lightLayer.update();
	EXPECT_EQ(lightLayer.getPlayState(), PATTERN_STARTED);
	usleep(25000);

	lightLayer.update();
	EXPECT_EQ(lightLayer.getPlayState(), PATTERN_PLAYING_IN_TRANSITION);
	usleep(1000);
	lightLayer.update();

	lightLayer.pause(true, false);
	EXPECT_EQ(lightLayer.getPlayState(), PATTERN_PAUSED);
	usleep(1000);
	lightLayer.update();

	lightLayer.unpause();
	EXPECT_EQ(lightLayer.getPlayState(), PATTERN_PLAYING_IN_TRANSITION);
	usleep(1000);
	lightLayer.update();

	usleep(50000);

	lightLayer.update();
	EXPECT_EQ(lightLayer.getPlayState(), PATTERN_PLAYING);
	usleep(50000);

	lightLayer.pause();
}

TEST_F(LightLayerTest, changedSequenceUpdatesPlayingPattern) {
	PatternSequence sequence1;
	PatternSequence sequence2;

	sequence1.addPatternCue(PatternCode(2, 0), 4000, TRANSITION_OVERWRITE, TRANSITION_FREEZE_FADE, 500);
	sequence1.addPatternCue(PatternCode(1, 0), -1);
	sequence1.addPatternCue(PatternCode(3, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);

	sequence2.addPatternCue(PatternCode(3, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);
	sequence2.addPatternCue(PatternCode(1, 0), -1);

	lightLayer.setPatternSequence(sequence1, 0);

	lightLayer.play();

	auto pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 2);

	lightLayer.setPatternSequence(sequence2, 1, 0);

	pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 1);
}

TEST_F(LightLayerTest, changedSequenceUpdatesPlayingPatternWithTransition) {
	PatternSequence sequence1;
	PatternSequence sequence2;

	sequence1.addPatternCue(PatternCode(2, 0), 500, TRANSITION_OVERWRITE, TRANSITION_FREEZE_FADE, 100);
	sequence1.addPatternCue(PatternCode(1, 0), 500, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 100);
	sequence1.addPatternCue(PatternCode(4, 0), 500, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 100);

	sequence2.addPatternCue(PatternCode(4, 0), 500, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 100);
	sequence2.addPatternCue(PatternCode(1, 0), 500, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 100);

	lightLayer.setPatternSequence(sequence1, 0);

	lightLayer.play();

	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);

	lightLayer.setPatternSequence(sequence2, 0, TRANSITION_PATTERN);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);

	runLayerFor(500);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 4);
}

TEST_F(LightLayerTest, changedSequenceStopsPlayingPattern) {
	PatternSequence sequence1;
	PatternSequence sequence2;

	sequence1.addPatternCue(PatternCode(2, 0), 4000, TRANSITION_OVERWRITE, TRANSITION_FREEZE_FADE, 500);
	sequence1.addPatternCue(PatternCode(1, 0), -1);
	sequence1.addPatternCue(PatternCode(3, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);

	sequence2.addPatternCue(PatternCode(3, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);
	sequence2.addPatternCue(PatternCode(1, 0), -1);

	lightLayer.setPatternSequence(sequence1, 0);

	lightLayer.play();

	auto pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 2);

	lightLayer.setPatternSequence(sequence2, -1, 0);
	EXPECT_EQ(lightLayer.isActive(), false);
}

TEST_F(LightLayerTest, changedSequenceSelectsPattern) {
	PatternSequence sequence1;
	PatternSequence sequence2;

	setup1Count = 0;

	sequence1.addPatternCue(PatternCode(2, 0), 4000, TRANSITION_OVERWRITE, TRANSITION_FREEZE_FADE, 500);
	sequence1.addPatternCue(PatternCode(1, 0), -1);
	sequence1.addPatternCue(PatternCode(3, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);

	sequence2.addPatternCue(PatternCode(2, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);
	sequence2.addPatternCue(PatternCode(1, 0), -1);

	lightLayer.setPatternSequence(sequence1);

	lightLayer.play();
	lightLayer.nextPattern();

	auto pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 1);

	lightLayer.setPatternSequence(sequence2, 1, 0);

	pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 1);

	EXPECT_EQ(setup1Count, 2);
}

TEST_F(LightLayerTest, changedSequenceStopsPlayback) {
	PatternSequence sequence1;
	PatternSequence sequence2;

	setup1Count = 0;

	sequence1.addPatternCue(PatternCode(2, 0), 4000, TRANSITION_OVERWRITE, TRANSITION_FREEZE_FADE, 500);
	sequence1.addPatternCue(PatternCode(1, 0), -1);
	sequence1.addPatternCue(PatternCode(3, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);

	sequence2.addPatternCue(PatternCode(2, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);
	sequence2.addPatternCue(PatternCode(1, 0), -1);

	lightLayer.setPatternSequence(sequence1, 0);
	runLayerFor(50);

	auto pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 2);

	lightLayer.setPatternSequence(sequence2, -1, 0);

	EXPECT_FALSE(lightLayer.isActive());
}

TEST_F(LightLayerTest, changedSequenceStopsPlaybackWitFade) {
	PatternSequence sequence1;
	PatternSequence sequence2;

	setup1Count = 0;

	sequence1.addPatternCue(PatternCode(2, 0), 4000, TRANSITION_OVERWRITE, TRANSITION_FREEZE_FADE, 500);
	sequence1.addPatternCue(PatternCode(1, 0), -1);
	sequence1.addPatternCue(PatternCode(3, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);

	sequence2.addPatternCue(PatternCode(2, 0), 500, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 100);
	sequence2.addPatternCue(PatternCode(1, 0), 500, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 100);

	lightLayer.setPatternSequence(sequence1, 0);
	runLayerFor(50);

	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);

	lightLayer.setPatternSequence(sequence2, -1, FADE_OUT);
	runLayerFor(50);
	EXPECT_EQ(lightLayer.getActivePattern()->getPatternID(), 2);
	EXPECT_TRUE(lightLayer.isActive());

	runLayerFor(150);
	EXPECT_FALSE(lightLayer.isActive());
}

TEST_F(LightLayerTest, changedSequenceStartsFromBeginIfPastEnd) {
	PatternSequence sequence1;
	PatternSequence sequence2;

	sequence1.addPatternCue(PatternCode(2, 0), 4000, TRANSITION_OVERWRITE, TRANSITION_FREEZE_FADE, 500);
	sequence1.addPatternCue(PatternCode(3, 0), -1);
	sequence1.addPatternCue(PatternCode(1, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);

	sequence2.addPatternCue(PatternCode(3, 0), 6000, TRANSITION_FADE_DOWN, TRANSITION_FREEZE_FADE, 700);
	sequence2.addPatternCue(PatternCode(1, 0), -1);

	lightLayer.setPatternSequence(sequence1, 0);

	lightLayer.play();
	lightLayer.prevPattern();

	auto pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 1);

	lightLayer.setPatternSequence(sequence2, 0, 0);

	pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern->getPatternID(), 3);
}

TEST_F(LightLayerTest, emptySequenceShouldJustStopPlayback) {
	PatternSequence sequence1;

	lightLayer.setPatternSequence(sequence1);

	lightLayer.play();
	lightLayer.prevPattern();

	EXPECT_EQ(lightLayer.getPlayState(), PATTERN_STOPPED);

	auto pattern = lightLayer.getActivePattern();
	EXPECT_EQ(pattern, nullptr);
}
