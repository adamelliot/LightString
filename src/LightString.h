#pragma once

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

// #define LS_VERBOSE

#include "utils.h"
#include "colortypes.h"
#include "PixelBuffer.h"

#include "palette.h"

#include "LightPattern.h"
#include "LightLayer.h"
#include "PatternManager.h"
#include "ThinPatternManager.h"

#define LIGHTSTRING_VERSION "0.6"
