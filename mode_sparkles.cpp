#include <Arduino.h>
#include <rgb_lcd.h>
#include <FastLED.h>
#include <Encoder.h>

#include "settings.h"
#include "utils.h"

extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];

// mode: 0 = fixed, 1 = slowly colour changing, 2 = random rainbow
void mode_sparkles(bool firstRun, int mode) {
	static long lastUpdate = 0;
	static int numLit = 0;
	static unsigned int hue = 0;
	const int MAX_SPEED = 10;
	const int MAX_CHANGE_SPEED = 10;
	// Counts how many sparkles for each new hue have been added
	// Used to add n sparkles for each hue when changing to slow the 
	// speed down
	static int hueSparklesAdded = 0;
	// Tracks the current hue colour we are adding
	static int currentHue = 0;
	
	int palettes[][5] = {
		{0,0,0,0,0},
		{110,120,130,140,150},		// blue-green
		{0,15,30,45,60},			//red-yellow
		{60,70,80,90,100},			//green-yellow
		{160,180,200,220,240}		// red blue
	};

	int hueChangeSpeed = 1;
	int desiredNumLit = getModeSetting(MODE_SPARKLES + mode, 0) * 10;	
	int speed = getModeSetting(MODE_SPARKLES + mode, 1);
	int sat = 255;
	int palette = 0;
	if (mode == 0) {	// fixed colour
		sat = getModeSetting(MODE_SPARKLES + mode, 3) * 10;
	}
	if (mode == 1) {	// changing colour
		palette = getModeSetting(MODE_SPARKLES + mode, 3);
	}


	// LEDs that are lit.  The LED numbers are in bins 0..numLit in a random order
	static uint16_t lit[NUM_LEDS];
	
	// LEDs that are unlit.  The LED numbers start out in bins 1..NUM_LEDs in order, but it gets randomised
	// as the mode runs
	static uint16_t unlit[NUM_LEDS];
	
	// Brightness scale (0-32,768) of each LED that is active.  The bin number matches the LED number
	// Used by sin16() fn to calculate the actual brightness of LED
	static uint16_t phase[NUM_LEDS];
	
	// Hue of each LED that is active
	static uint8_t hues[NUM_LEDS];

	
	if (firstRun) {
		// Reset everything

		for (int i = 0; i < NUM_LEDS; i++) {
			unlit[i] = i;
			lit[i] = 0;
			phase[i] = 0;
			hues[i] = 0;
		}
		
		
		// Randomise the arrays at the start.  The basic idea is to choose an "unlit" LED at random, copy it's
		// number to the "lit" array, and then remove it from the "unlit" array.  We "remove" it, by shifting
		// all the values above it down by one bin, and putting 0 at the top.
		for (int i = 0; i < desiredNumLit; i++) {
			int bin = random16(NUM_LEDS - i);	// Choose an unlit LED from the remaining ones (NUM_LEDS - i)
			lit[i] = unlit[bin];				// Move it to the lit array
			phase[unlit[bin]] = random8(64) * 512; // Give it a random start value that is a multiple of 512
			switch (mode) {
			case 0:
				hues[unlit[bin]] = getModeSetting(MODE_SPARKLES + mode, 2) * 10;
				break;
			case 1:
				if (palette == 0) {	// palette 0 is changing through all colours, so start at 0;
					hue = 0;
				}  else {
					hues[unlit[bin]] = palettes[palette][random8(0, 5)];
				}
				break;
			case 2:
				hues[unlit[bin]] = random8();
				break;
			}
			for (int j = bin; j < NUM_LEDS - 1; j++) {
				unlit[j] = unlit[j+1];
			}
			unlit[NUM_LEDS - 1] = 0;
		}
		numLit = desiredNumLit;
	}

	// "Number Lit" dial
	if (desiredNumLit > numLit) {
		// Going up
		for (int i = 0; i < desiredNumLit - numLit; i++) {
			if (numLit < NUM_LEDS) {
				int bin = random16(NUM_LEDS - numLit);	// Choose a random unlit led
				lit[numLit] = unlit[bin];		// Move it to the lit array
				phase[unlit[bin]] = random8(64) * 512; //randomize it's value
				switch (mode) {
				case 0:
					hues[unlit[bin]] = getModeSetting(MODE_SPARKLES, 2) * 10;
					break;
				case 1:
					if (palette == 0) {	// palette 0 is changing through all colours
						hues[unlit[bin]] = hue;
					} else {
						hues[unlit[bin]] = palettes[palette][random8(0, 5)];
					}
					break;
				case 2:
					hues[unlit[bin]] = random8();
					break;
				}

				// and remove it from the unlit array
				for (int j = bin; j < NUM_LEDS - 1; j++) {
					unlit[j] = unlit[j + 1];
				}
				numLit++;
			}
		}
	} else if (desiredNumLit < numLit) {	// going down...
		int difference = numLit - desiredNumLit;
		p("going down, different %u\n", difference);
		for (int i = 0; i < difference; i++) {
			if (numLit > 0) {  // going down
				unlit[NUM_LEDS - numLit] = lit[numLit]; // move the top most "lit" led to the unlit array
				phase[lit[numLit]] = 0;	// and turn it out etc.
				lit[numLit] = 0;
				numLit--;
			}
		}
	}

	
	
	if  (millis() - lastUpdate > 200 / speed ) {
		memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
		lastUpdate = millis();
		for (int i = 0; i < numLit; i++) {
			leds[lit[i]] = CHSV(hues[lit[i]], sat, sin16(phase[lit[i]]) >> 8);
			phase[lit[i]] += 512;
			if (phase[lit[i]] > 32767) { // One of them has gone out,
				// We randomly choose a new LED from the unlit array, and swap it with the lit one that has gone out.
				// Over time the arrays will both become randomly ordered, but this doesn't matter.
				int temp = lit[i];	// Save the lit LED number
				int bin = random16(NUM_LEDS - numLit);	// Choose a new random bin
				lit[i] = unlit[bin]; // And swap them over
				unlit[bin] = temp;
				phase[lit[i]] = 0;	// Set the new value to 0;

				switch (mode) {
				case 0:
					hues[lit[i]] = getModeSetting(MODE_SPARKLES, 2) * 10;
					break;
				case 1: {
					/*
					We want to add sparkles so that the colour changes at a constant
					rate regardless of the speed of the leds twinkling.

					*/
					int changeSpeed = getModeSetting(MODE_SPARKLES + mode, 2);
					if (hueSparklesAdded <= 3 * (MAX_CHANGE_SPEED - changeSpeed)) {
						// We need to add sparkles of the existing hue
						hueSparklesAdded = hueSparklesAdded + (MAX_SPEED - speed) + 1;
					}
					else {
						hueSparklesAdded = 1;
						if (palette == 0) {	// palette 0 is changing through all colours
							currentHue++;
							hue = currentHue;
							if (currentHue > 256) currentHue = 0;
						}
						else {
							hue = palettes[palette][random8(0, 5)];
						}
					}
					hues[lit[i]] = hue;

					}break;
				case 2:
					hues[lit[i]] = random8();
					break;
				}
			}
		}

		// Boost the brightness
		int brightness = constrain(2*getSetting(GLOBAL_SETTINGS,2),0,255);
		LEDS.setBrightness(brightness);
		LEDS.show();
	}
}
