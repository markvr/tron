#include <Arduino.h>
#include <rgb_lcd.h>
#include <FastLED.h>

#include "settings.h"
#include "utils.h"
#include "mode_fixed.h"


extern CRGB leds[NUM_LEDS];

void mode_fixed(bool firstRun) {
	int hue = getModeSetting(MODE_FIXED, 0) * 10;
	int sat = getModeSetting(MODE_FIXED, 1) * 10;
	int sparkles = getModeSetting(MODE_FIXED, 2) * 3;
	int sparklesSpeed = 10 - getModeSetting(MODE_FIXED, 3);
	int brightness = constrain(2 * getSetting(GLOBAL_SETTINGS, 2), 0, 255);
	static long lastUpdate = millis();
	if (millis() - lastUpdate > 20 * sparklesSpeed) {
		for (int i = 0; i < NUM_LEDS; i++) {
			leds[i] = CHSV(hue, sat, brightness);
		}

		for (int i = 0; i < sparkles; i++) {
			int led = random16(0, NUM_LEDS);
			leds[led] = CHSV(0, 0, 255);
		}
		
		
		LEDS.setBrightness(255);

		LEDS.show();
		lastUpdate = millis();
	}
}

