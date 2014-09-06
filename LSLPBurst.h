#ifndef _LSLPBURST_H_
#define _LSLPBURST_H_

#include "LSLightString.h"

#define BURST 0x0a
#define MAX_BURSTS 8

typedef struct burst_s burst_t, *pburst_t;
struct burst_s {
	uint16_t index;
	uint8_t step;
//	float step;
//	float stepRate;
	uint8_t totalSteps;
	uint8_t colorIndex;

	uint8_t direction; // 0x1 left, 0x2 right, 0x3 both
};

class LSLPBurst : public LSLightProgram {
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
	LSLPBurst(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc)
		: LSLightProgram(pixelBuffer, colorPalette, colorFunc) {
		modeCount = 5;
	}
	uint8_t getProgramID() { return BURST; }

	void setupMode(uint8_t mode);
	void update(uint32_t ms);
};

LSLightProgram *factoryBurst(LSPixelBuffer *pixelBuffer, LSColorPalette* colorPalette, pcolor_func colorFunc);

#endif
