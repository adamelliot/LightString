#ifndef _LSLIGHTSTRING_H_
#define _LSLIGHTSTRING_H_

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

#include <EEPROM.h>

// Core Functionality

#include "palette.h"

#include "LSPixelBuffer.h"
#include "LSLightProgram.h"
#include "ProgramManager.h"
/*
// Palettes
#include "LSCPSolidPalette.h"
#include "LSCPGradientPalette.h"
*/
// Programs
#include "LSLPFadeDown.h"
#include "LSLPGradientCycle.h"
#include "LSLPTwinkle.h"
#include "LSLPSolidCycle.h"
#include "LSLPBounce.h"
#include "LSLPBreathe.h"
#include "LSLPPulse.h"
#include "LSLPStrobe.h"
#include "LSLPSolidColor.h"
#include "LSLPBurst.h"
#include "LSLPHeartBeat.h"
#include "LSLPCustomProgram.h"

/*

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
