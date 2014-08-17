#include "settings.h"
#include "Arduino.h"
#include "rgb_lcd.h"
#include "FastSPI_LED2.h"
#include <Encoder.h>
#include "utils.h"
#include "mode_fixed.h"

extern Encoder dialOne;
extern Encoder dialTwo;
extern Encoder dialBrightness;
extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];

void mode_fixed(bool firstRun) {
	static long dialOneOldPosition = 0;
	static int hue = getSetting(0,1);
	boolean hueChanged = false;
	if (hue < 0 || hue > 250) hue = 0; // prevent crazy hues, not sure what comes out if eeprom not initd
	long dialOneNewPosition =  dialOne.read();
	if (firstRun) dialOneOldPosition = dialOneNewPosition;
	if (dialOneNewPosition % 4 == 0 && dialOneNewPosition != dialOneOldPosition) {
		int dir = (dialOneNewPosition - dialOneOldPosition) / 4;
		dialOneOldPosition = dialOneNewPosition;
		hue += 10* dir;
		if (hue < 0) hue = 250;
		if (hue > 250) hue = 0;
		setSetting(0,1,hue);
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