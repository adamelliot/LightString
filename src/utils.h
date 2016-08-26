#pragma once

#include <math.h>
#include <stdint.h>

#ifndef ARDUINO

#include <cstdlib>
#include <queue>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

uint64_t millis();
long random(long max);
long random(long min, long max);

#else

#define usleep delayMicroseconds
// #define swap(x, y) do { decltype(x) SWAP = x; x = y; y = SWAP; } while (0)

#endif

namespace LightString {
template <typename T>
T map(T x, T in_min, T in_max, T out_min, T out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
};

#define mapf LightString::map<float>

inline float clamp(float value, float min, float max) {
	return ((value < min) ? min : (value > max) ? max : value);
}

#ifdef USE_FASTLED
inline void blink(CRGB col = CRGB::Yellow, int times = 4, int timing = 75) {
	CRGB saveCol = *FastLED.leds();

	for (int i = 0; i < times; i++) {
		*FastLED.leds() = col;
		FastLED.show();
		delay(timing);
		FastLED.showColor(CRGB::Black);
		delay(timing >> 1);
	}

	*FastLED.leds() = saveCol;
}
#endif

