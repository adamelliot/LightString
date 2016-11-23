#include <cstdlib>
#include "utils.h"

#ifndef ARDUINO

uint64_t millis() {
    timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

long random(long max) {
    return std::rand() % max;
}

long random(long min, long max) {
    return (std::rand() % (max - min)) + min;
}

#else

#include <Arduino.h>

// Fix Teensy issues with stl
namespace std {
void __throw_bad_alloc()
{
	Serial.println("Unable to allocate memory");
}

void __throw_length_error(char const *e)
{
	Serial.print("Length Error :");
	Serial.println(e);
}
};

#endif
