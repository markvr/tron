#include "settings.h"
#include "Arduino.h"
#include "rgb_lcd.h"
#include "FastSPI_LED2.h"
#include "utils.h"

extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];

void mode_rainbow(bool firstRun) {
	static int density = getSetting(3,1);
	static int speed = getSetting(3,2);
	static int offset = 0;
	int updateTimeGap = 100;
	static unsigned long lastUpdate = 0;
	boolean displayChanged = false;
	
	if (density < 1 || density > 32) density = 1;
	if (speed < 1 || speed > 10) speed = 1;
	
	int dialOneChanged = getDialOne();
	if (dialOneChanged) {
		density += dialOneChanged;
		density = constrain(density, 1, 32);
		setSetting(3,1,density);
		displayChanged = true;
	}

	int dialTwoChanged = getDialTwo();
	if (dialTwoChanged) {
		speed += dialTwoChanged;
		speed = constrain(speed, 1, 10);
		setSetting(3,2,speed);
		displayChanged = true;
	}
	
	if (displayChanged || firstRun) {
		char string[16];
		sprintf(string, "dty:%i, spd:%i", density, speed);
		printLcd(1, string);
	}

	if( (millis() - lastUpdate) > (1000 / (speed * speed))) {
		
		offset += 5;
		lastUpdate = millis();
		for (int i = 0; i < NUM_LEDS; i++) {
			int hue = (i * density) / 4  + offset;
			leds[i].setHue(hue);
		}
		LEDS.show();
	}
}