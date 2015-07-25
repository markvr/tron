#include <rgb_lcd.h>
#include <FastLED.h>
#include <Encoder.h>

#include "settings.h"
#include "utils.h"
#include "LocalAudio.h"

extern CRGB leds[NUM_LEDS];

void mode_volume(bool firstRun) {
	if (firstRun) LocalAudio::start();
	static long lastUpdate = 0;
	static int maxVolume = 0;
	int sensitivity = getModeSetting(MODE_VOLUME, 0);
	int fadeRate = getModeSetting(MODE_VOLUME, 1);
	
//	const int ribbons[][2] = { { 0, 15 }, { 16, 35 }, { 36, 50 }, { 51, 65 }, { 66, 81 }, { 82, 101 }, { 102, 116 }, { 117, 131 }, { 132, 142 }, { 143, 151 }, { 152, 160 }, { 161, 168 }, { 169, 179 }, { 180, 188 }, { 189, 197 }, { 198, 205 }, { 206, 218 }, { 219, 230 }, { 231, 240 }, { 241, 254 }, { 255, 266 }, { 267, 285 }, { 286, 296 } };
//	const int numRibbons = 23;

	// 267

	if( (millis() - lastUpdate) > (100)) {
		lastUpdate = millis();
		memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));

		unsigned int volume = LocalAudio::getVolume();
		volume = sqrt(volume);  // divide by 16
		Serial.println(volume);
	
		for (int i = 285; i > 285 - volume; i--){
			leds[i].setHSV(50, 255, 255);
		}
		// Let the LEDs write in peace
		LocalAudio::stop();
		LEDS.show();
		LocalAudio::start();

	}



}
