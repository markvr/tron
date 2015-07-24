#include <Arduino.h>
#include <rgb_lcd.h>
#include <FastLED.h>

#include "settings.h"
#include "utils.h"
#include "mode_fixed.h"


extern CRGB leds[NUM_LEDS];

void mode_fixed(bool firstRun) {
	int hue = getModeSetting(MODE_FIXED,0) * 10;
	int sat = getModeSetting(MODE_FIXED,1) * 10;
	for(int i = 0; i < NUM_LEDS; i++) {
		leds[i] = CHSV( hue, sat , 255);
	}
	LEDS.show();
}
