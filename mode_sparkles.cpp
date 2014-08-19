#include "settings.h"
#include "Arduino.h"
#include "rgb_lcd.h"
#include "FastSPI_LED2.h"
#include <Encoder.h>
#include "utils.h"

extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];

void mode_sparkles(bool firstRun, bool rainbow) {
	static long lastUpdate = 0;
	const int hue = getSetting(MODE_FIXED,1);	// Get the hue from the "fixed" mode.
	bool displayChanged = false;

	const int changeByNumber = 10;

	int numLit = getSetting(MODE_SPARKLES,1);	// Number of LEDs that are lit at a time
	if (numLit < 0 || numLit > NUM_LEDS) numLit = 0;

	int speed = getSetting(MODE_SPARKLES,2);
	if (speed < 1 || speed > 10) speed = 1;
	
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
		for (int i = 0; i < numLit; i++) {
			int bin = random16(NUM_LEDS - i);	// Choose an unlit LED from the remaining ones (NUM_LEDS - i)
			lit[i] = unlit[bin];				// Move it to the lit array
			phase[unlit[bin]] = random8(64) * 512; // Give it a random start value that is a multiple of 512
			if (rainbow) {
				hues[unlit[bin]] = random8();
				} else {
				hues[unlit[bin]] = hue;
			}
			// Shift the rest of the array down
			for (int j = bin; j < NUM_LEDS - 1; j++) {
				unlit[j] = unlit[j+1];
			}
			unlit[NUM_LEDS - 1] = 0;
		}
	}
	
	// "Number Lit" dial
	int numLitChange = getDialOne();
	if (numLitChange && (NUM_LEDS > (changeByNumber * numLitChange))) { // Stop going > NUM_LEDS
		if (numLitChange == 1) {	// Going Up
			for (int i = 0; i < changeByNumber; i++) {	// Increase by 10 at a time
				if (numLit < NUM_LEDS) {	
					int bin = random16(NUM_LEDS - numLit);	// Choose a random unlit led
					lit[numLit] = unlit[bin];		// Move it to the lit array
					phase[unlit[bin]] =  random8(64) * 512; //randomize it's value
					if (rainbow) {
						hues[unlit[bin]] = random8();
					} else {
						hues[unlit[bin]] = hue;
					}
					// and remove it from the unlit array
					for (int j = bin; j < NUM_LEDS - 1; j++) {
						unlit[j] = unlit[j+1];
					}
					numLit++;
				}
			}
		} else {	// going down...
			for (int i = 0; i < changeByNumber; i++) {
				if (numLit > 0) {  // going down
					unlit[NUM_LEDS - numLit] = lit[numLit]; // move the top most "lit" led to the unlit array
					phase[lit[numLit]] = 0;	// and turn it out etc.
					lit[numLit] = 0;
					numLit--;
				}
			}
		}
		
		setSetting(MODE_SPARKLES,1, numLit);
		displayChanged = true;
	}
	
	int speedChange = getDialTwo();
	if (speedChange) {
		speed += speedChange;
		speed = constrain(speed, 1, 10);
		setSetting(MODE_SPARKLES,2,speed);
		displayChanged = true;
	}
	
	
	
	if (displayChanged || firstRun) {
		char string[16];
		sprintf(string, "lit:%i spd:%i ", numLit, speed);
		printLcd(1, string);
	}
	
	
	if  (millis() - lastUpdate > 200 / speed ) {
		memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
		lastUpdate = millis();
		for (int i = 0; i < numLit; i++) {
			//leds[lit[i]] = CRGB(0, 0, sin16(brightness[lit[i]]) >> 8);
			leds[lit[i]] = CHSV(hues[lit[i]], 255, sin16(phase[lit[i]]) >> 8);
			phase[lit[i]] += 2048;
			if (phase[lit[i]] > 32767) { // One of them has gone out,
				// We randomly choose a new LED from the unlit array, and swap it with the lit one that has gone out.
				// Over time the arrays will both become randomly ordered, but this doesn't matter.
				int temp = lit[i];	// Save the lit LED number
				int bin = random16(NUM_LEDS - numLit);	// Choose a new random bin
				lit[i] = unlit[bin]; // And swap them over
				unlit[bin] = temp;
				phase[lit[i]] = 0;	// Set the new value to 0;
				if (rainbow) {
					hues[lit[i]] = random8();
				} else {
					hues[lit[i]] = hue;
				}
				
			}
		}

		// Boost the brightness
		int brightness = constrain(2*getSetting(GLOBAL_SETTINGS,2),0,255);
		LEDS.setBrightness(brightness);
		LEDS.show();
	}
}