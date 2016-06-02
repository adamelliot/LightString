#pragma once

#ifndef _LIGHTSTRING_H_
#define _LIGHTSTRING_H_

#ifdef ARDUINO
#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#include <pins_arduino.h>
#endif
#endif

#ifdef USE_FASTLED
#include <FastLED.h>
#endif




#include "colortypes.h"
#include "palette.h"
#include "drawing.h"

#include "LightProgram.h"
#include "ProgramManager.h"

#define LIGHTSTRING_VERSION "0.5"

#endif
