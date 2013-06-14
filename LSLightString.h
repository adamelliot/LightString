#ifndef _LSLIGHTSTRING_H_
#define _LSLIGHTSTRING_H_

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

// Core Functionality

#include "LSColorPalette.h"
#include "LSPixelBuffer.h"
#include "LSLEDStrip.h"
#include "LSLightProgram.h"
#include "LSLightProgramManager.h"

// Palettes
#include "LSCPGradientPalette.h"

// Programs
#include "LSLPFadeDown.h"
#include "LSLPRainbowCycle.h"

/*
#include "LSLPTwinkle.h"
#include "LSLPSolidCycle.h"
#include "LSLPBounce.h"
#include "LSLPPulse.h"

#include "LSLP2DLines.h"
#include "LSLPFire.h"
#include "LSLPPlasma.h"

#include "LSLPSoundStrip.h"

// Poi Programs

#include "LSLPPoiText.h"
#include "LSLPPoiCheckers.h"
#include "LSLPPoiGears.h"
*/

#endif
