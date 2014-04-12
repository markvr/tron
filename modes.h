// modes.h

#ifndef _MODES_h
#define _MODES_h

#include "Arduino.h"
#include "rgb_lcd.h"
#include "FastSPI_LED2.h"
#include <Encoder.h>
#define NUM_LEDS 20


void fixed(Encoder dialOne, Encoder dialTwo);
void fade();
void rainbow();
void chasing();

#endif

