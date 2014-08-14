#ifndef _MODE_HELMET_h
#define _MODE_HELMET_h

#include "settings.h"
#include "Arduino.h"
#include "rgb_lcd.h"
#include "FastSPI_LED2.h"
#include <Encoder.h>
#include "utils.h"
#define ARM_MATH_CM4
#include <arm_math.h>
#include "Audio.h"

extern Encoder dialOne;
extern Encoder dialTwo;
extern Encoder dialBrightness;
extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];

void mode_helmet(bool firstRun);

#endif

