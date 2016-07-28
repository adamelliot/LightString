#pragma once

#include <sys/time.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <queue>
#include <cstdlib>
#include <stdint.h>

#ifndef ARDUINO

uint64_t millis();
long random(long max);
long random(long min, long max);

#endif

template <typename T>
T map(T x, T in_min, T in_max, T out_min, T out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline float clamp(float value, float min, float max) {
  return ((value < min)? min : (value > max)? max : value);
}

// Older tools

// #define SWAP(x, y) do { decltype(x) SWAP = x; x = y; y = SWAP; } while (0)

// extern const uint8_t kMatrixWidth;
// extern const uint8_t kMatrixHeight;

// extern uint8_t *generic_map;

extern int16_t xy(int16_t, int16_t);

#ifdef USE_FASTLED
void blink(CRGB col = CRGB::Yellow, int times = 4, int timing = 75);
#endif
