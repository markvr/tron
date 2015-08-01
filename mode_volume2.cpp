#include <rgb_lcd.h>
#include <FastLED.h>
#include <Encoder.h>

#include "settings.h"
#include "utils.h"
#include "LocalAudio.h"

extern CRGB leds[NUM_LEDS];



void mode_volume2(bool firstRun) {
	static int frames = 0;
	static int lastFrameUpdate = 0;
	if (firstRun) {
		LocalAudio::start();
		frames = 0;
	}
	static unsigned int volume;
	static unsigned int oldVolume = 0;
	static long lastUpdate = 0;
	static int maxVolume = 0;
	int sensitivity = getModeSetting(MODE_VOLUME2, 0);
	int fadeRate = getModeSetting(MODE_VOLUME2, 1);

	//	const int ribbons[][2] = { { 0, 15 }, { 16, 35 }, { 36, 50 }, { 51, 65 }, { 66, 81 }, { 82, 101 }, { 102, 116 }, { 117, 131 }, { 132, 142 }, { 143, 151 }, { 152, 160 }, { 161, 168 }, { 169, 179 }, { 180, 188 }, { 189, 197 }, { 198, 205 }, { 206, 218 }, { 219, 230 }, { 231, 240 }, { 241, 254 }, { 255, 266 }, { 267, 285 }, { 286, 296 } };
	//	const int numRibbons = 23;
	// 206
	const int maxRibbonsPerThreshold = 5;
	int numThresholds = 4;
	const int ribbons[][maxRibbonsPerThreshold][2] = {
		{ { 267, 285 } },
		{ { 286, 296 } },
		{ { 206, 218 } },
		{ { 219, 230 } }
	};

	const int red = 0;
	const int orange = 50;
	const int yellow = 70;
	const int green = 100;
	
	int volumePerThreshold = 48;	//512 / num thresholds

	// {volume, colour, num ribbons}
	 int thresholds[][3] = { 
		{ 64, green, 1 }, 
		{ 128, yellow, 1 }, 
		{ 256, orange, 1 }, 
		{ 512, red, 1 } 
	};

	if ((millis() - lastUpdate) > 50) {
		frames++;
		if (millis() - lastFrameUpdate > 1000) {
			lastFrameUpdate = millis();
			p("fps %u\n", frames);
			frames = 0;
		}
		lastUpdate = millis();
		memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));

		unsigned int newVolume = LocalAudio::getVolume(true);
		if (newVolume > volume) {
			volume = newVolume;
		}  else {
			volume = volume * ((20.0 - (float)fadeRate) / 20.0);
		}	
		p("volume %u \n", volume);
		for (int threshold = 0; threshold < numThresholds; threshold++) {
			int startVolume = threshold * volumePerThreshold;
			int endVolume = (threshold + 1) * volumePerThreshold;

			int brightness;
			if (volume > endVolume) {
				brightness = 255; 				// set full brightness
			} else {
				brightness = volume - startVolume;
			}
			p("\t threshold %u, startVolume %u  endVolume %u, brightness %u\n", threshold, startVolume, endVolume, brightness);
			if (brightness < 50) brightness = 50;
			for (int ribbon = 0; ribbon < thresholds[threshold][2]; ribbon++) {
				p("\t\t ribbon %u, startLed %u  endLed %u, brightness %u \n", threshold, ribbons[threshold][ribbon][0], ribbons[threshold][ribbon][1], brightness);
				for (int led = ribbons[threshold][ribbon][0]; led < ribbons[threshold][ribbon][1]; led++) {
//					p("\t\t\tsetting led %u to %u, endLed %u\n", led, brightness, ribbons[threshold][ribbon][1]);
					leds[led].setHSV(thresholds[threshold][1], 255, brightness);
				}
			}

		}

		//for (int i = 285; i > 285 - ledCount; i--){
		//	leds[i].setHSV(50, 255, 255);
		//}

		// Let the LEDs write in peace
		LocalAudio::stop();
		LEDS.show();
		LocalAudio::start();

	}

	



}
