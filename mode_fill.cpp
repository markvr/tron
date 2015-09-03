#include <Arduino.h>
#include <rgb_lcd.h>
#include <FastLED.h>

#include "settings.h"
#include "utils.h"
#include "mode_fixed.h"

extern CRGB leds[NUM_LEDS];

void mode_fill(boolean firstRun) {
	int density = 10 - getModeSetting(MODE_FILL, 0);
	int speed = 2 * (10 - getModeSetting(MODE_FILL, 1));
	static long lastUpdate = millis();
	static int dir = 1;

	int ribbons[][2] = {
		{ 0, 15 }, // upper right leg inside
		{ 16, 35 }, // upper right leg outside
		{ 36, 50 }, // lower right leg outside 
		{ 51, 65 },  // lower right leg inside
		{ 66, 81 },  // upper left leg inside
		{ 82, 101 }  // upper left leg outside
	};

	int ribbon[] = { 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, };
	const int length = 31;
	static int led[length];
	static int top = 0;
	static int bottom = 0;
	if (firstRun) {
		memset(&led[0], 0, sizeof(led));
		top = 0;
	}

	static long lastLed = millis();
	int delay = 1;
	int timeGap = density * 100 * delay;

	if (millis() - lastUpdate > delay) {
		lastUpdate = millis();
		if (dir == 1) {		// filling up
			for (int i = top; i < length; i++) {
				if (led[i] > 0) {
					led[i]--;
					if (led[i] == 0)  {
						if (i - 1 <= top) {
							led[top] = 1;
							top++;
						}
						else {
							led[i - 1] = max(1, (1 - ((float)(i - top) / (float)(length - top))) * speed);
						}
					}
				}
			}
			if (top == length) {
				top = 0;
				dir = -1;
				memset(&led[0], 1, sizeof(led));
			}
		} else {	// emptying
			p("emptying\n");
			for (int i = 0; i < bottom; i++) {
				if (led[i] > 0) {
					led[i]--;
					if (led[i] == 0 && i > 0)  {
						led[i - 1] = max(1, (1 - ((float)(bottom - i) / (float)(bottom))) * speed);
					}

				}
			}
		}
		if (millis() - lastLed > timeGap) {
			lastLed = millis();
			if (dir == 1) {
				led[length - 1] = 1;
			} else {
				bottom++;
				if (bottom == length) {
					dir = 1;
				}
			}
//			p("new led\n");
		}
		p("\n");

		memset(&leds[0], 0, sizeof(leds));
		for (int i = 0; i < length; i++) {
			if (led[i] > 0) {
				leds[ribbon[i]] = CHSV(0, 0,  255);
			}
				p("%u ", led[i]);
		}
			p("\n");



		LEDS.show();
	}

}
