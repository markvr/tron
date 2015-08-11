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
	int maxVolume = 1000 - (100 * getModeSetting(MODE_VOLUME, 0)) + 1;
	int fadeRate = getModeSetting(MODE_VOLUME, 1);
	int palette = getModeSetting(MODE_VOLUME, 2);
	
	const int ribbons[][2] = { { 0, 15 }, { 16, 35 }, { 36, 50 }, { 51, 65 }, { 66, 81 }, { 82, 101 }, { 102, 116 }, { 117, 131 }, { 132, 142 }, { 143, 151 }, { 152, 160 }, { 161, 168 }, { 169, 179 }, { 180, 188 }, { 189, 197 }, { 198, 205 }, { 206, 218 }, { 219, 230 }, { 231, 240 }, { 241, 254 }, { 255, 266 }, { 267, 285 }, { 286, 296 } };
	const int numRibbons = 23;
	const int palettes[][26] = {
		{ 100, 100, 100, 100, 100, 100, 100, 70, 70, 70, 70, 70, 70, 0, 0, 0, 0, 0 }, // green-orange-red
		{ 180, 184, 186, 190, 192, 196, 200, 202, 206, 210, 212, 214, 216, 220, 222, 226, 228, 230 }, // blue-pink
		{ 100, 110, 130, 140, 160, 170, 190, 200, 220, 230,  250, 0, 20, 30, 50, 60, 70, 80} // rainbow
	};



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

		
		Serial.println(volume);
	
		for (int ribbon = 0; ribbon < numRibbons; ribbon++) {
			int start = ribbons[ribbon][0];
			int end = ribbons[ribbon][1];
			int size = end - start;
			int ledCount = ((float)volume / (float)maxVolume) * size;
			for (int i = end; i > max(start, end - ledCount); i--){
				leds[i].setHSV(palettes[palette][end - i], 255, 255);
			}
		}

		// Let the LEDs write in peace
		LocalAudio::stop();
		LEDS.show();
		LocalAudio::start();

	}



}
