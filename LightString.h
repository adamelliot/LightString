#ifndef _LIGHTSTRING_H_
#define _LIGHTSTRING_H_

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

#include <EEPROM.h>

// Core Functionality

#include "Palette.h"

#include "PixelBuffer.h"
#include "LightProgram.h"
#include "ProgramManager.h"

// Programs
#include "FadeDown.h"
#include "GradientCycle.h"
#include "Twinkle.h"
#include "SolidCycle.h"
#include "Bounce.h"
#include "Breathe.h"
#include "Pulse.h"
#include "Strobe.h"
#include "SolidColor.h"
#include "Burst.h"
#include "HeartBeat.h"
#include "CustomProgram.h"

/*

#include "2DLines.h"
#include "Fire.h"
#include "Plasma.h"

#include "SoundStrip.h"

// Poi Programs

#include "PoiText.h"
#include "PoiCheckers.h"
#include "PoiGears.h"
*/

using namespace LightString;

#endif
