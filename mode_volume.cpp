#include <rgb_lcd.h>
#include <FastLED.h>
#include <Encoder.h>

#include "settings.h"
#include "utils.h"
#include "LocalAudio.h"

extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];

void mode_volume(bool firstRun) {
	if (firstRun) LocalAudio::start();
	static long lastUpdate = 0;
	bool displayChanged = false;
	static int maxVolume = 0;
	static float hue = 0;
	static int sensitivity = getSetting(MODE_VOLUME, 1);
	static int fadeRate = getSetting(MODE_VOLUME, 2);
	
	int dialOneChanged = getDialOne();
	if (dialOneChanged) {
		sensitivity += dialOneChanged * 5;
		sensitivity = constrain(sensitivity, 1, 50);
		setSetting(MODE_VOLUME,1,sensitivity);
		displayChanged = true;
	}

	int dialTwoChanged = getDialTwo();
	if (dialTwoChanged) {
		fadeRate += dialTwoChanged;
		dialTwoChanged = constrain(dialTwoChanged, 1, 10);
		setSetting(MODE_VOLUME,2,fadeRate);
		displayChanged = true;
	}
	
	if (displayChanged || firstRun) {
		char string[16];
		sprintf(string, "sty :%i, spd:%i", sensitivity, fadeRate);
		printLcd(1, string);
	}


	
	if( (millis() - lastUpdate) > (100)) {
		lastUpdate = millis();
		memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));

		float volume = LocalAudio::getVolume();
		float newHue = sqrt(volume) * sensitivity;
		if (newHue > hue) {
			hue = newHue;
		} else {
			hue = hue * (float)fadeRate / 10;
		}
		p("hue: %f \n", hue);
		if (hue > 256) hue = 256;
		
		for (int i = 0; i < NUM_LEDS; i++) {
			leds[i].setHue((int)hue);
		}
		LEDS.show();
	}
}
