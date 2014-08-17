#include "settings.h"
#include "Arduino.h"
#include "rgb_lcd.h"
#include "FastSPI_LED2.h"
#include <Encoder.h>
#include "utils.h"


extern Encoder dialOne;
extern Encoder dialTwo;
extern Encoder dialBrightness;
extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];

void mode_rainbow(bool firstRun) {
	static long dialOneOldPosition = 0;
	static long dialTwoOldPosition = 0;
	static int density = getSetting(3,1);
	static int speed = getSetting(3,2);
	static int offset = 0;
	int updateTimeGap = 100;
	static unsigned long lastUpdate = 0;
	boolean displayChanged = false;
	
	if (density < 1 || density > 32) density = 1;
	if (speed < 1 || speed > 10) speed = 1;
	
	long dialOneNewPosition =  dialOne.read();
	if (firstRun) dialOneOldPosition = dialOneNewPosition;
	if (dialOneNewPosition % 4 == 0 && dialOneNewPosition != dialOneOldPosition) {
		int dir = (dialOneNewPosition - dialOneOldPosition) / 4;
		dialOneOldPosition = dialOneNewPosition;
		density += dir;
		density = constrain(density, 1, 32);
		setSetting(3,1,density);
		displayChanged = true;
	}

	long dialTwoNewPosition =  dialTwo.read();
	if (firstRun) dialTwoOldPosition = dialTwoNewPosition;
	if (dialTwoNewPosition % 4 == 0 && dialTwoNewPosition != dialTwoOldPosition) {
		int dir = (dialTwoNewPosition - dialTwoOldPosition) / 4;
		dialTwoOldPosition = dialTwoNewPosition;
		speed += dir;
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