#ifndef _LIGHTSTRING_H_
#define _LIGHTSTRING_H_

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

// Core Functionality

#include "palette.h"
#include "drawing.h"

#include "LightProgram.h"
#include "ProgramManager.h"

#define LIGHTSTRING_VERSION "0.4"

using namespace LightString;

#endif
