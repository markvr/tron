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
	static int sat = getSetting(MODE_FIXED,2);
	boolean displayChanged = false;
		
	int dialOneChanged = getDialOne();
	if (dialOneChanged) {
		hue += 10* dialOneChanged;
		if (hue < 0) hue = 250;
		if (hue > 250) hue = 0;
		setSetting(MODE_FIXED,1,hue);
		displayChanged = true;
	}
	
	int dialTwoChanged = getDialTwo();
	if (dialTwoChanged) {
		sat += 10 * dialTwoChanged;
		sat = constrain(sat,0,250);
		setSetting(MODE_FIXED,2,sat);
		displayChanged = true;
	}
	
	if (displayChanged || firstRun) {
		char string[16];
		sprintf(string, "hue:%i,sat:%i", hue,sat);
		printLcd(1, string);

		for(int i = 0; i < NUM_LEDS; i++) {
			leds[i] = CHSV( hue, sat , 255);
		}
		LEDS.show();
	}

}