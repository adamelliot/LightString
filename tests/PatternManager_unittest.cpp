#include "gtest/gtest.h"

#include <stdlib.h>

#include "helpers.h"
#include "../src/PatternManager.h"

using namespace LightString;

template <template <typename> class PIXEL, typename FORMAT>
class TestPixelBufferProvider {
private:
	TPixelBuffer<PIXEL, FORMAT> buffer = TPixelBuffer<PIXEL, FORMAT>(5);

public:
	virtual ~TestPixelBufferProvider() {}

	virtual int bufferSize() { return 5; }
	virtual TPixelBuffer<PIXEL, FORMAT> *requestBuffer() {
		return &buffer;
	}
	virtual void releaseBuffer(TPixelBuffer<PIXEL, FORMAT> *) {}

};

class TestPatternProvider : public PatternProvider {
public:
	virtual ILightPattern *patternForID(pattern_id_t patternID, ILightLayer *layer = nullptr) {
		switch (patternID) {
			case 1: return new TSolidColorPattern<TRGB, float>(HTML::Red);
		}

		return nullptr;
	}
};

TEST(PatternManager, initialization) {
	TestPatternProvider provider;
	PatternManager<TRGB, uint8_t> patternManager(provider);

	EXPECT_EQ(patternManager.getSection(0), nullptr);
}

TEST(PatternManager, sectionsExist) {
	TestPatternProvider provider;
	PatternManager<TRGB, uint8_t> patternManager(provider);

	uint8_t sectionID = patternManager.addLightSection(5);

	auto section = patternManager.getSection(sectionID);
	EXPECT_EQ(section->getSectionID(), sectionID);
	EXPECT_EQ(patternManager.getSection(1), nullptr);
}

TEST(PatternManager, clearingAllSections) {
	TestPatternProvider provider;
	PatternManager<TRGB, uint8_t> patternManager(provider);

	uint8_t sectionID = patternManager.addLightSection(5);
	EXPECT_EQ(sectionID, 0);

	auto section = patternManager.getSection(sectionID);
	EXPECT_EQ(section->getSectionID(), sectionID);
	EXPECT_EQ(patternManager.getSection(1), nullptr);

	patternManager.clearLightSections();

	EXPECT_EQ(patternManager.getSection(0), nullptr);

	sectionID = patternManager.addLightSection(5);
	EXPECT_EQ(sectionID, 0);
	section = patternManager.getSection(sectionID);
	EXPECT_EQ(section->getSectionID(), sectionID);
}

TEST(PatternManager, startRunningUsingPlay) {
	TestPatternProvider provider;
	PatternManager<TRGB, uint8_t> patternManager(provider);

	uint8_t sectionID = patternManager.addLightSection(5);

	auto section = patternManager.getSection(sectionID);
	EXPECT_EQ(section->getSectionID(), sectionID);
	EXPECT_EQ(patternManager.getSection(1), nullptr);

	patternManager.addLightPattern(1);
	patternManager.play();

	patternManager.update();
}

TEST(PatternManager, startRunningUsingRandom) {
	TestPatternProvider provider;
	PatternManager<TRGB, uint8_t> patternManager(provider);

	uint8_t sectionID = patternManager.addLightSection(5);

	auto section = patternManager.getSection(sectionID);
	EXPECT_EQ(section->getSectionID(), sectionID);
	EXPECT_EQ(patternManager.getSection(1), nullptr);

	patternManager.addLightPattern(1);
	patternManager.startRandomPattern();

	patternManager.update();
}

TEST(PatternManager, ensureLayersGetConfigured) {
	TestPatternProvider provider;
	PatternManager<TRGB, uint8_t> patternManager(provider);

	uint8_t sectionID = patternManager.addLightSection(5);

	patternManager.setPatternDuration(500);

	auto section = patternManager.getSection(sectionID);
	EXPECT_EQ(section->getSectionID(), sectionID);

	patternManager.addLightPattern(1);

	EXPECT_EQ(section->getTotalLayers(), 1);
	EXPECT_EQ(section->getLayer(0)->getPatternDuration(), 500);

	patternManager.play();

	patternManager.update();
}
