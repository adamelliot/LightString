#include "utils.h"

#ifndef ARDUINO

uint64_t millis() {
    timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

long random(long max) {
    return rand() % max;
}

long random(long min, long max) {
    return (rand() % (max - min)) + min;
}

#endif
