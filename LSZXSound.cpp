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

	raw >>= 3;

	return raw;
}

int16_t LSZXSound::getMagnitude() {
	uint16_t level = this->getLevel();
/*	Serial.print("> AVG: ");
	Serial.println(level);
	
	Serial.print("> RAW: ");
	Serial.println(rawValues[rawIndex]);
	
	Serial.println(rawValues[rawIndex] - level);
	Serial.println("---");
*/
	if (rawValues[rawIndex] < level) {
//		Serial.println("A: 0");
		return 0;
	} else {
		Serial.print("B: ");
		Serial.println(rawValues[rawIndex] - level);
		return rawValues[rawIndex] - level;//(uint8_t)(rawValues[rawIndex] - level);
	}
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
	
	//this->getMagnitude();
	
	/*
	Serial.print("RAW: ");
	Serial.println(this->getRawLevel());
	
	Serial.print(" AVG: ");
	Serial.println(this->getLevel());
	
	Serial.print(" ADJUSTED: ");
	Serial.println(this->getAdjustedLevel());
	*/
//	Serial.println(" MAG: ");
//	Serial.println(this->getMagnitude());

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
