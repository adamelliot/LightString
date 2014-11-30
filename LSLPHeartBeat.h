#ifndef _LSLPHEARTBEAT_H_
#define _LSLPHEARTBEAT_H_

#include "LSLightString.h"

#define HEART_BEAT 0x0b

class LSLPHeartBeat : public LSLightProgram {
protected:
	uint8_t colorIndex;
	uint8_t changeRate;
	uint8_t offset;

	uint16_t msPerBeat, msDubOffset;
	uint32_t lastTime;
	uint32_t nextLub, nextDub;

public:
	LSLPHeartBeat(LSPixelBuffer *pixelBuffer)
		: LSLightProgram(pixelBuffer) {
		modeCount = 1;
	}
	uint8_t getProgramID() { return HEART_BEAT; }

	void setupMode(uint8_t mode);
	void update(uint32_t ms);

};

LSLightProgram *factoryHeartBeat(LSPixelBuffer *pixelBuffer);

#endif
