#include "LSZXSound.h"

LSZXSound::LSZXSound(int pin) : pin(pin), count(0), rawIndex(0), maxIndex(0) {
	for (int i = 0; i < SOUND_VALUES; i++) {
		rawValues[i] = SOUND_RESET_VAL;
		maxValues[i] = SOUND_RESET_VAL;
	}
}

uint16_t LSZXSound::getRawLevel() {
	return rawValues[rawIndex];
}

uint16_t LSZXSound::getLevel() {
	uint16_t raw;
	for (int i = 0; i < SOUND_VALUES; i++) {
		raw += rawValues[i];
	}

	raw >>= 2;
	return raw;
}

float LSZXSound::getAdjustedLevel() {
	uint16_t raw = 0, max = 0;

	for (int i = 0; i < SOUND_VALUES; i++) {
		raw += rawValues[i];
		max += maxValues[i];
	}

	return min((float)raw / (float)max, 1.0f);
}

void LSZXSound::update() {
	uint16_t rawLevel = analogRead(pin);

	if (rawLevel == 0) return;
	/*
	Serial.print("RAW: ");
	Serial.print(this->getLevel());

	Serial.print(" ADJUSTED: ");
	Serial.println(this->getAdjustedLevel());
*/
	rawValues[rawIndex++] = rawLevel;
	rawIndex %= SOUND_VALUES;

	if (rawLevel > maxValues[maxIndex])
		maxValues[maxIndex] = rawLevel;
	
	if (count++ > SOUND_RESET_CYCLES) {
		count = 0;
		maxIndex++;
		maxIndex %= SOUND_VALUES;

		maxValues[maxIndex] = SOUND_RESET_VAL;
	}
}
