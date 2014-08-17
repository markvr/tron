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

void mode_chasing(bool firstRun) {
	static unsigned long lastUpdate = 0;
	static int level = 0;
	static int dir = 1;
	int led;
	static long dialOneOldPosition = 0;
	boolean speedChanged = false;
	static int speed = 0;

	long dialOneNewPosition =  dialOne.read();
	if (firstRun) {
		dialOneOldPosition = dialOneNewPosition;
		led = 0;
	}
	if (dialOneNewPosition % 4 == 0 && dialOneNewPosition != dialOneOldPosition) {
		int dir = (dialOneNewPosition - dialOneOldPosition) / 4;
		dialOneOldPosition = dialOneNewPosition;
		speed +=  dir;
		speedChanged = true;
	}
	
	if (speedChanged || firstRun) {
		char string[16];
		sprintf(string, "speed: %i", speed);
		printLcd(1, string);
	}
	

	memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
	leds[speed] = CRGB(128, 0, 0);
	
	
	
	LEDS.show();

}