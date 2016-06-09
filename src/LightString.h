#pragma once

#ifndef _LIGHTSTRING_H_
#define _LIGHTSTRING_H_

#ifdef ARDUINO
#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WPattern.h>
#include <pins_arduino.h>
#endif
#endif

#ifdef USE_FASTLED
#include <FastLED.h>
#endif

#include "utils.h"
#include "colortypes.h"

// #include "palette.h"
// #include "drawing.h"

#include "LightPattern.h"
#include "LightLayer.h"
#include "PatternManager.h"

#define LIGHTSTRING_VERSION "0.5"

#endif
