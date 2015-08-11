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
	int maxVolume = 1000 - (100 * getModeSetting(MODE_VOLUME2, 0)) + 1;
//	int sensitivity = getModeSetting(MODE_VOLUME2, 0);
	int fadeRate = getModeSetting(MODE_VOLUME2, 1);

	//	const int ribbons[][2] = { { 0, 15 }, { 16, 35 }, { 36, 50 }, { 51, 65 }, { 66, 81 }, { 82, 101 }, { 102, 116 }, { 117, 131 }, { 132, 142 }, { 143, 151 }, { 152, 160 }, { 161, 168 }, { 169, 179 }, { 180, 188 }, { 189, 197 }, { 198, 205 }, { 206, 218 }, { 219, 230 }, { 231, 240 }, { 241, 254 }, { 255, 266 }, { 267, 285 }, { 286, 296 } };
	//	const int numRibbons = 23;
	// 206
	const int maxRibbonsPerThreshold = 5;
	int numThresholds = 4;
	const int ribbons[][maxRibbonsPerThreshold][2] = {
		{ { 36, 50 }, { 51, 65 }, { 102, 116 }, { 117, 131 } },	 //lower leg
		{ { 0, 15 }, { 16, 35 }, { 66, 81 }, { 82, 101 } },	// upper leg
		{ { 152, 160 }, { 161, 168 }, { 189, 197 }, { 198, 205 } },
		{ { 132, 142 }, { 143, 151 }, { 169, 179 }, {180,188 } }
	};

	const int red = 0;
	const int orange = 50;
	const int yellow = 70;
	const int green = 100;
	
	int volumePerThreshold = 48;	//512 / num thresholds

	// {volume, colour, num ribbons}
	 int thresholds[][3] = { 
		{ 64, green, 4 }, 
		{ 128, yellow, 4 }, 
		{ 256, orange, 4 }, 
		{ 512, red, 4 } 
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


			/*volume 96
				threshold 0, startVolume 0  endVolume 48, brightness 255
				ribbon 0, startLed 36  endLed 50, brightness 255
				ribbon 0, startLed 51  endLed 65, brightness 255
				ribbon 0, startLed 102  endLed 116, brightness 255
				ribbon 0, startLed 117  endLed 131, brightness 255
				threshold 1, startVolume 48  endVolume 96, brightness 48
				ribbon 1, startLed 0  endLed 15, brightness 50
				ribbon 1, startLed 16  endLed 35, brightness 50
				ribbon 1, startLed 66  endLed 81, brightness 50
				ribbon 1, startLed 82  endLed 101, brightness 50
				threshold 2, startVolume 96  endVolume 144, brightness 0
				ribbon 2, startLed 152  endLed 160, brightness 50
				ribbon 2, startLed 161  endLed 168, brightness 50
				ribbon 2, startLed 189  endLed 197, brightness 50
				ribbon 2, startLed 198  endLed 205, brightness 50
				threshold 3, startVolume 144  endVolume 192, brightness 4294967248
				ribbon 3, startLed 132  endLed 142, brightness 50
				ribbon 3, startLed 143  endLed 151, brightness 50
				ribbon 3, startLed 169  endLed 179, brightness 50
				ribbon 3, startLed 180  endLed 188, brightness 50

*/

			int brightness;
			if (volume > endVolume) {
				brightness = 255; 				// set full brightness
			} else {	// fix brightness fading
				brightness = 255 - volume - startVolume;
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
