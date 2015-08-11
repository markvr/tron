#include <Arduino.h>
#include <rgb_lcd.h>
#include <FastLED.h>

#include "settings.h"
#include "utils.h"
#include "mode_fixed.h"

extern CRGB leds[NUM_LEDS];

void mode_count() {
	int led = getModeSetting(MODE_COUNT, 0);
	for (int i = 0; i < NUM_LEDS; i++) {
		leds[i] = 0;
	}

	leds[led] = CHSV(255, 0, 255);
	LEDS.show();

}