#include <Arduino.h>
#include <rgb_lcd.h>
#include <FastLED.h>

#include "settings.h"
#include "utils.h"

extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];

void mode_rainbow(bool firstRun) {
	int density = getModeSetting(MODE_RAINBOW,0);
	int speed = getModeSetting(MODE_RAINBOW,1);

	static int offset = 0;
	int updateTimeGap = 100;
	static unsigned long lastUpdate = 0;

	if( (millis() - lastUpdate) > (200 / (speed * speed))) {
		p("density %u \n", density);
		offset += (speed / 5) + 1;
		lastUpdate = millis();
		for (int i = 0; i < NUM_LEDS; i++) {
			int hue = (i * density) / 4  + offset;
			leds[i].setHSV(hue, 255, 255);
		}
		LEDS.show();
	}
}
