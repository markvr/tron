#include "settings.h"
#include "Arduino.h"
#include "rgb_lcd.h"
#include "FastSPI_LED2.h"
#include "utils.h"

extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];

void mode_falling(bool firstRun) {
	const int MODE_NUMBER = 3;
	static unsigned long lastUpdate = 0;
	
	const int hue = getSetting(0,1);	// Get the hue from the "fixed" mode.
	const int ribbons[][2] = { {0,15},	{16,35},	{36,50},	{51,65},	{66,81},	{82,101},	{102,116},	{117,131},	{132,142},	{143,151},	{152,160},	{161,168},	{169,179},	{180,188},	{189,197},	{198,205},	{206,218},	{219,230},	{231,240},	{241,254},	{255,266},	{267,285},	{286,296}};	
	const int numRibbons = 23;
	
	bool displayChanged = false;

	static int delay[numRibbons] = {0};
	static int positions[numRibbons] = {0};
	static int tailLength =  5; //getSetting(3,1);;
	static int speed = 5; //getSetting(3,2);;
	
	if (firstRun) {
		for (int i = 0; i < numRibbons; i++) {
			positions[i]=random(ribbons[i][0], ribbons[i][1]);
		}
	}

	const int saturationDecrement = (int) (255.0 / (float)tailLength);

	int updateTimeGap = 250 / speed;
	

	int tailLengthChange = getDialOne();
	if (tailLengthChange) {
		tailLength += tailLengthChange;
		tailLength = constrain(tailLength, 0, 10);
		setSetting(3,1,tailLength);
		displayChanged = true;

	}

	int speedChange = getDialTwo();
	if (speedChange) {
		speed += speedChange;
		speed = constrain(speed, 0, 10);
		setSetting(3,2,speed);
		displayChanged = true;
	}
	
	if (displayChanged ) {
		char string[16];
		sprintf(string, "lgth:%i, spd:%i", tailLength, speed);
		printLcd(1, string);
	}
	displayChanged = false;

	if( (millis() - lastUpdate) > updateTimeGap) {
		
		for (int i = 0; i < NUM_LEDS; i++) {
			leds[i] = 0;
		}
		
		
		for (int ribbonNumber = 0; ribbonNumber < numRibbons; ribbonNumber++) {
			if (delay[ribbonNumber] == 0) {
				int firstLed = ribbons[ribbonNumber][0];
				int lastLed = ribbons[ribbonNumber][1];
				int length = lastLed - firstLed;
				int position = positions[ribbonNumber];
				int sat = 255;
				for (int i = 0; i < tailLength; i++) {
					int led = position - i;
					if (led >= 0) {
						led = led + firstLed;
						leds[led] = CHSV( hue, sat ,  sat);
						sat = sat - saturationDecrement;
					}
				}
				positions[ribbonNumber] = (position + 1);
				if (positions[ribbonNumber] == length + 1) {
					positions[ribbonNumber] = 0;
					delay[ribbonNumber] = random(5,20);
				}
			} else {
				delay[ribbonNumber]--;
			}
		}
		
		
		
		LEDS.show();
		lastUpdate = millis();
	}
	
}