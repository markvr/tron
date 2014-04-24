// modes.h

#ifndef _MODES_h
#define _MODES_h
#include "settings.h"
#include "Arduino.h"
#include "rgb_lcd.h"
#include "FastSPI_LED2.h"
#include <Encoder.h>
#include "utils.h"


extern Encoder dialOne;
extern Encoder dialTwo;
extern Encoder dialBrightness;
extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];


void fixed(boolean);
void fade(boolean);
void rainbow(boolean);
void chasing(boolean);
void sparkles(boolean, boolean);

#endif

