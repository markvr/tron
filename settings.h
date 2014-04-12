// settings.h

#ifndef _SETTINGS_h
#define _SETTINGS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


#define MODE_PIN 12
#define NUM_MODES 4
#define SETTINGS_PER_MODE 3

#endif

