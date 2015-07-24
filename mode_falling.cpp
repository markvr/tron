#include <Arduino.h>
#include <rgb_lcd.h>
#include <FastLED.h>

#include "settings.h"
#include "utils.h"

extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];

void mode_falling(bool firstRun) {
	const int MODE_NUMBER = 3;
	static unsigned long lastUpdate = 0;
	

	const int ribbons[][2] = { {0,15},	{16,35},	{36,50},	{51,65},	{66,81},	{82,101},	{102,116},	{117,131},	{132,142},	{143,151},	{152,160},	{161,168},	{169,179},	{180,188},	{189,197},	{198,205},	{206,218},	{219,230},	{231,240},	{241,254},	{255,266},	{267,285},	{286,296}};
	const int numRibbons = 23;
	
	static int startDelay[numRibbons] = {0};
	static int positions[numRibbons] = {0};
	int tailLength =  getModeSetting(MODE_FALLING,0);
	int speed = getModeSetting(MODE_FALLING,1);
	int hue = getModeSetting(MODE_FALLING, 2) * 10;
	int sat = getModeSetting(MODE_FALLING, 3) * 10;
	
	if (firstRun) {
		for (int i = 0; i < numRibbons; i++) {
			positions[i]= random(0, ribbons[i][1] - ribbons[i][0]);
		}
	}

	const int saturationDecrement = (int) (255.0 / (float)tailLength);

	if( (millis() - lastUpdate) >  250 / speed) {
		for (int i = 0; i < NUM_LEDS; i++) {
			leds[i] = 0;
		}
		
		
		for (int ribbonNumber = 0; ribbonNumber < numRibbons; ribbonNumber++) {
			if (startDelay[ribbonNumber] == 0) {
				int firstLed = ribbons[ribbonNumber][0];
				int lastLed = ribbons[ribbonNumber][1];
				int length = lastLed - firstLed;
				int position = positions[ribbonNumber];
				int tail_sat = sat;
				int tail_val = 255;
				for (int i = 0; i < tailLength; i++) {
					int led = position - i;
					if (led >= 0) {
						led = led + firstLed;
						leds[led] = CHSV(hue, tail_sat, tail_val);
						tail_sat = constrain(tail_sat - saturationDecrement, 0, 255);
						tail_val = tail_val - saturationDecrement;
					}
				}
				positions[ribbonNumber] = (position + 1);
				if (positions[ribbonNumber] == length + 1) {
					positions[ribbonNumber] = 0;
					startDelay[ribbonNumber] = random(5,20);
				}
				} else {
				startDelay[ribbonNumber]--;
			}
		}
		
		int globalBrightness = constrain(2 * getSetting(GLOBAL_SETTINGS, 2), 0, 255);
		LEDS.setBrightness(globalBrightness);
		
		LEDS.show();
		lastUpdate = millis();
	}
	
}
