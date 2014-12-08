#ifndef _HEARTBEAT_H_
#define _HEARTBEAT_H_

#include "LightString.h"

#define HEART_BEAT 0x0b

namespace LightString {

class HeartBeat : public LightProgram {
protected:
	uint8_t colorIndex;
	uint8_t changeRate;
	uint8_t offset;

	uint16_t msPerBeat, msDubOffset;
	uint32_t lastTime;
	uint32_t nextLub, nextDub;

public:
	HeartBeat(PixelBuffer *pixelBuffer = 0)
		: LightProgram(pixelBuffer) {
		modeCount = 1;
	}
	uint8_t getProgramID() { return HEART_BEAT; }

	void setupMode(uint8_t mode);
	void update(uint32_t ms);

};

};

#endif
