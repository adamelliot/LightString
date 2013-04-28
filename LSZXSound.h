#ifndef _LSZXSOUND_H_
#define _LSZXSOUND_H_

#include <Arduino.h>

#define SOUND_VALUES 8
#define SOUND_RESET_VAL 8
#define SOUND_RESET_CYCLES 30

class LSZXSound {
private:
	int pin;
	uint8_t count, maxIndex, rawIndex;

	uint16_t rawValues[SOUND_VALUES];
	uint16_t maxValues[SOUND_VALUES];

public:
	LSZXSound(int pin);

	float getAdjustedLevel();
	int getMagnitude();
	uint16_t getLevel();
	uint16_t getRawLevel();

	void update();
};

#endif
