#include "settings.h"
#include "Arduino.h"
#include "rgb_lcd.h"
#include "FastSPI_LED2.h"
#include "utils.h"
#include "mode_fixed.h"

extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];

void mode_fixed(bool firstRun) {
	static int hue = getSetting(MODE_FIXED,1);
	boolean hueChanged = false;
	if (hue < 0 || hue > 250) hue = 0; // prevent crazy hues, not sure what comes out if eeprom not initd
	
	int dialOneChanged = getDialOne();
	if (dialOneChanged) {
		hue += 10* dialOneChanged;
		if (hue < 0) hue = 250;
		if (hue > 250) hue = 0;
		setSetting(MODE_FIXED,1,hue);
		hueChanged = true;
	}
	
	if (hueChanged || firstRun) {
		char string[16];
		sprintf(string, "hue: %i", hue);
		printLcd(1, string);

		for(int i = 0; i < NUM_LEDS; i++) {
			leds[i].setHue(hue);
		}
		LEDS.show();
	}

}