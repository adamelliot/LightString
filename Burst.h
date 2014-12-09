#ifndef _BURST_H_
#define _BURST_H_

#include "LightString.h"

#define BURST 0x0a
#define MAX_BURSTS 9

namespace LightString {

typedef struct burst_s burst_t, *pburst_t;
struct burst_s {
	uint16_t index;
	uint8_t step;
	uint8_t totalSteps;
	uint8_t colorIndex;

	uint8_t direction; // 0x1 left, 0x2 right, 0x3 both
};

class Burst : public LightProgram {
protected:
	bool fade;
	bool mirrored;
	bool oneWay;
	bool left, right;

	float fadeRate;
	uint8_t colorIndex;
	uint8_t changeRate;
	uint8_t changeJump;

	uint8_t minBurstLength;
	burst_t bursts[MAX_BURSTS];
	
	void addBurst();

public:
	Burst(PixelBuffer *pixelBuffer = 0)
		: LightProgram(pixelBuffer) {
		modeCount = 5;
	}
	uint8_t getProgramID() { return BURST; }

	void setupMode(uint8_t mode);
	void update(uint32_t ms);
};

};

#endif
