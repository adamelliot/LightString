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
