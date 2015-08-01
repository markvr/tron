#include <rgb_lcd.h>
#include <FastLED.h>
#include <Encoder.h>

#include "settings.h"
#include "utils.h"
#include "LocalAudio.h"

extern CRGB leds[NUM_LEDS];

void mode_volume(bool firstRun) {
	static int frames = 0;
	static int lastFrameUpdate = 0;
	if (firstRun) {
		LocalAudio::start();
		frames = 0;
	}
	static unsigned int volume = 0;
	static long lastUpdate = 0;
	static int maxVolume = 0;
	int sensitivity = getModeSetting(MODE_VOLUME, 0);
	int fadeRate = getModeSetting(MODE_VOLUME, 1);
	
//	const int ribbons[][2] = { { 0, 15 }, { 16, 35 }, { 36, 50 }, { 51, 65 }, { 66, 81 }, { 82, 101 }, { 102, 116 }, { 117, 131 }, { 132, 142 }, { 143, 151 }, { 152, 160 }, { 161, 168 }, { 169, 179 }, { 180, 188 }, { 189, 197 }, { 198, 205 }, { 206, 218 }, { 219, 230 }, { 231, 240 }, { 241, 254 }, { 255, 266 }, { 267, 285 }, { 286, 296 } };
//	const int numRibbons = 23;

	// 267
	int start = 285;
	int end = 267;
	int size = 285 - 267; // = 18
	if ((millis() - lastUpdate) > 50) {
		frames++;
		if (millis() - lastFrameUpdate > 1000) {
			lastFrameUpdate = millis();

			p("fps %u\n", frames);
			frames = 0;
		}
		lastUpdate = millis();
		memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));

		unsigned int newVolume = LocalAudio::getVolume(true);
		if (newVolume > volume) {
			volume = newVolume;
		} else {
			volume = volume * ( (20.0 - (float)fadeRate) / 20.0);
		}

		int ledCount = volume / size;
		//Serial.println(volume);
	
		for (int i = 285; i > 285 - ledCount; i--){
			leds[i].setHSV(50, 255, 255);
		}

		// Let the LEDs write in peace
		LocalAudio::stop();
		LEDS.show();
		LocalAudio::start();

	}



}
