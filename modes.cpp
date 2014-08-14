#include "modes.h"

void _shuffle(int *array, int n) {
	int i, j, tmp;

	for (i = n - 1; i > 0; i--) {
		j = random8(i + 1);
		tmp = array[j];
		array[j] = array[i];
		array[i] = tmp;
	}
}

void fixed(boolean firstRun) {
	static long dialOneOldPosition = 0;
	static int hue = getSetting(0,1);
	boolean hueChanged = false;
	if (hue < 0 || hue > 250) hue = 0; // prevent crazy hues, not sure what comes out if eeprom not initd
	long dialOneNewPosition =  dialOne.read();
	if (firstRun) dialOneOldPosition = dialOneNewPosition;
	if (dialOneNewPosition % 4 == 0 && dialOneNewPosition != dialOneOldPosition) {
		int dir = (dialOneNewPosition - dialOneOldPosition) / 4;
		dialOneOldPosition = dialOneNewPosition;
		hue += 10* dir;
		if (hue < 0) hue = 250;
		if (hue > 250) hue = 0;
		setSetting(0,1,hue);
		hueChanged = true;
	}
	if (hueChanged || firstRun) {
		char string[16];
		sprintf(string, "hue: %i", hue);
		printLcd(1, string);

		for(int i = 0; i < NUM_LEDS; i++) {
			leds[i].setHue(hue);
		}
		LEDS.show();
	}

}



//void chasing(boolean firstRun) {
	//static unsigned long lastUpdate = 0;
	//static int level = 0;
	//static int dir = 1;
	//static int currentLED = 0;
	//static long dialOneOldPosition = 0;
	//static int speed = getSetting(1,1);
	//boolean speedChanged = false;
//
	//if (speed < 10 || speed > 100) speed = 10; 
	//long dialOneNewPosition =  dialOne.read();
	//if (firstRun) dialOneOldPosition = dialOneNewPosition;
	//if (dialOneNewPosition % 4 == 0 && dialOneNewPosition != dialOneOldPosition) {
		//int dir = (dialOneNewPosition - dialOneOldPosition) / 4;
		//dialOneOldPosition = dialOneNewPosition;
		//speed += 10* dir;
		//if (speed < 10) speed = 10;
		//if (speed > 100) speed = 100;
		//setSetting(1,1,speed);
		//speedChanged = true;
	//}
	//
	//if (speedChanged || firstRun) {
		//char string[16];
		//sprintf(string, "speed: %i", speed);
		//printLcd(1, string);
	//}
	//
	//
	//if ( (millis() - lastUpdate > 10000 / speed) ) {
		//memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
		//leds[currentLED] = CRGB(128, 0, 0);
		//currentLED = (currentLED + dir) % NUM_LEDS;
		//if (currentLED == 0 || currentLED == NUM_LEDS - 1) dir = dir * -1;
		//p("LED: %u \n", currentLED);
		//lastUpdate = millis();
		//LEDS.show();
	//}
//
//}


void chasing(boolean firstRun) {
	static unsigned long lastUpdate = 0;
	static int level = 0;
	static int dir = 1;
	int led;
	static long dialOneOldPosition = 0;
	boolean speedChanged = false;
	static int speed = 0;

	long dialOneNewPosition =  dialOne.read();
	if (firstRun) {
		dialOneOldPosition = dialOneNewPosition;
		led = 0;
	}
	if (dialOneNewPosition % 4 == 0 && dialOneNewPosition != dialOneOldPosition) {
		int dir = (dialOneNewPosition - dialOneOldPosition) / 4;
		dialOneOldPosition = dialOneNewPosition;
		speed +=  dir;
		speedChanged = true;
	}
	
	if (speedChanged || firstRun) {
		char string[16];
		sprintf(string, "speed: %i", speed);
		printLcd(1, string);
	}
	

	memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
	leds[speed] = CRGB(128, 0, 0);
	
	
	
	LEDS.show();

}

void fade(boolean firstRun) {
	static long dialOneOldPosition = 0;
	static long dialTwoOldPosition = 0;
	static int hue = getSetting(2,1);
	static unsigned long lastUpdate = 0;
	int updateTimeGap = 5;
	static int level = 0;
	static int dir = 1;
	if (hue < 0 || hue > 250) hue = 0; // prevent crazy hues, not sure what comes out if eeprom not initd
	long dialOneNewPosition =  dialOne.read();
	
	
	if (dialOneNewPosition % 4 == 0 && dialOneNewPosition != dialOneOldPosition) {
		int dir = (dialOneNewPosition - dialOneOldPosition) / 4;
		dialOneOldPosition = dialOneNewPosition;
		hue += 10 * dir;
		if (hue < 0) hue = 250;
		if (hue > 250) hue = 0;
		setSetting(2,1,hue);
	}

	if( (millis() - lastUpdate) > updateTimeGap) {
		memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
		level = (level + dir) % 128;
		if (level == 0 || level == 127) dir = dir * -1;
		for(int i = 0; i < NUM_LEDS; i++) {
			leds[i] = CRGB(0, 0, level);
		}

		LEDS.show();
		lastUpdate = millis();
	}
}

void rainbow(boolean firstRun) {
	static long dialOneOldPosition = 0;
	static long dialTwoOldPosition = 0;
	static int density = getSetting(3,1);
	static int speed = getSetting(3,2);
	static int offset = 0;
	int updateTimeGap = 100;
	static unsigned long lastUpdate = 0;
	boolean displayChanged = false;
	
	if (density < 1 || density > 32) density = 1; 		
	if (speed < 1 || speed > 10) speed = 1; 		
	
	long dialOneNewPosition =  dialOne.read();
	if (firstRun) dialOneOldPosition = dialOneNewPosition;
	if (dialOneNewPosition % 4 == 0 && dialOneNewPosition != dialOneOldPosition) {
		int dir = (dialOneNewPosition - dialOneOldPosition) / 4;
		dialOneOldPosition = dialOneNewPosition;
		density += dir;
		density = constrain(density, 1, 32);
		setSetting(3,1,density);
		displayChanged = true;
	}

	long dialTwoNewPosition =  dialTwo.read();
	if (firstRun) dialTwoOldPosition = dialTwoNewPosition;
	if (dialTwoNewPosition % 4 == 0 && dialTwoNewPosition != dialTwoOldPosition) {
		int dir = (dialTwoNewPosition - dialTwoOldPosition) / 4;
		dialTwoOldPosition = dialTwoNewPosition;
		speed += dir;
		speed = constrain(speed, 1, 10);
		setSetting(3,2,speed);
		displayChanged = true;
	}


	
	if (displayChanged || firstRun) {
		char string[16];
		sprintf(string, "dty:%i, spd:%i", density, speed);
		printLcd(1, string);
	}

	if( (millis() - lastUpdate) > (1000 / (speed * speed))) {
		
		offset += 5;
		lastUpdate = millis();
		for (int i = 0; i < NUM_LEDS; i++) {
			int hue = (i * density) / 4  + offset;
			leds[i].setHue(hue);
		}
		LEDS.show();
	}
}



void sparkles(boolean firstRun, boolean rainbow) {
	static long dialOneOldPosition = 0;
	static long dialTwoOldPosition = 0;
	static long lastUpdate = 0;
	const int hue = getSetting(0,1);	// Get the hue from the "fixed" mode.
	bool displayChanged = false;

	int numLit = getSetting(4,1);	// Number of LEDs that are lit at a time
	if (numLit < 0 || numLit > NUM_LEDS) numLit = 0;

	int speed = getSetting(4,2);
	if (speed < 1 || speed > 10) speed = 1; 
	
	// LEDs that are lit.  The LED numbers are in bins 0..numLit in a random order
	static uint16_t lit[NUM_LEDS];	
	
	// LEDs that are unlit.  The LED numbers start out in bins 1..NUM_LEDs in order, but it gets randomised
	// as the mode runs
	static uint16_t unlit[NUM_LEDS];	
	
	// Brightness scale (0-32,768) of each LED that is active.  The bin number matches the LED number
	// Used by sin16() fn to calculate the actual brightness of LED
	static uint16_t brightness[NUM_LEDS];
	
	// Hue of each LED that is active
	static uint8_t hues[NUM_LEDS];

	
	if (firstRun) {
		// Reset everything
		for (int i = 0; i < NUM_LEDS; i++) {
			unlit[i] = i;
			lit[i] = 0;
			brightness[i] = 0;
			hues[i] = 0;
		}
		
		
		// Randomise the arrays at the start.  The basic idea is to choose an "unlit" LED at random, copy it's
		// number to the "lit" array, and then remove it from the "unlit" array.  We "remove" it, by shifting
		// all the values above it down by one bin, and putting 0 at the top.
		for (int i = 0; i < numLit; i++) {
			int bin = random16(NUM_LEDS - i);	// Choose an unlit LED from the remaining ones (NUM_LEDS - i)
			lit[i] = unlit[bin];				// Move it to the lit array
			brightness[unlit[bin]] = random8(64) * 512; // Give it a random start value that is a multiple of 512	
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
	long dialOneNewPosition =  dialOne.read();
	if (firstRun) dialOneOldPosition = dialOneNewPosition; // Reset the old and new positions to the same
	if (dialOneNewPosition % 4 == 0 && dialOneNewPosition != dialOneOldPosition) {
		int dir = (dialOneNewPosition - dialOneOldPosition) / 4;
		dialOneOldPosition = dialOneNewPosition;
		
		if (dir == 1) {	// Going Up
			for (int i = 0; i < 10; i++) {
				if (numLit < NUM_LEDS) {	// Similar to above:
					int bin = random16(NUM_LEDS - numLit);	// Choose a random unlit led
					lit[numLit] = unlit[bin];		// Move it to the lit array
					brightness[unlit[bin]] =  random8(64) * 512; //randomize it's value
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
		} else {
			for (int i = 0; i < 10; i++) {
				if (numLit > 0) {  // going down
					unlit[NUM_LEDS - numLit] = lit[numLit]; // move the top most "lit" led to the unlit array
					brightness[lit[numLit]] = 0;	// and turn it out etc.
					lit[numLit] = 0;
					numLit--;
				}
			}
		}
		
		setSetting(4,1, numLit);
		displayChanged = true;
	}
	
	long dialTwoNewPosition =  dialTwo.read();
	if (firstRun) dialTwoOldPosition = dialTwoNewPosition;
	if (dialTwoNewPosition % 4 == 0 && dialTwoNewPosition != dialTwoOldPosition) {
		int dir = (dialTwoNewPosition - dialTwoOldPosition) / 4;
		dialTwoOldPosition = dialTwoNewPosition;
		speed += dir;
		speed = constrain(speed, 1, 10);
		setSetting(4,2,speed);
		displayChanged = true;
	}
	
	
	
	if (displayChanged || firstRun) {
		char string[16];
		sprintf(string, "lit:%i spd:%i ", numLit, speed);
		printLcd(1, string);
	}
	
	
	if( (millis() - lastUpdate) > 200 / ( speed * speed)) {
		memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
		lastUpdate = millis();
		for (int i = 0; i < numLit; i++) {
			//leds[lit[i]] = CRGB(0, 0, sin16(brightness[lit[i]]) >> 8);
			leds[lit[i]] = CHSV(hues[lit[i]], 255, sin16(brightness[lit[i]]) >> 8);
			brightness[lit[i]] += 512;
			if (brightness[lit[i]] > 32767) { // One of them has gone out, 
				// We randomly choose a new LED from the unlit array, and swap it with the lit one that has gone out.
				// Over time the arrays will both become randomly ordered, but this doesn't matter.
				int temp = lit[i];	// Save the lit LED number
				int bin = random16(NUM_LEDS - numLit);	// Choose a new random bin
				lit[i] = unlit[bin]; // And swap them over
				unlit[bin] = temp;
				brightness[lit[i]] = 0;	// Set the new value to 0;
				if (rainbow) {
					hues[lit[i]] = random8();
				} else {
					hues[lit[i]] = hue;
				}
				
			}
		}
				
		LEDS.show();
	}
}
	
void falling(boolean firstRun) {
		static long dialOneOldPosition = 0;
		static long dialTwoOldPosition = 0;
		static unsigned long lastUpdate = 0;
		
		const int hue = getSetting(0,1);	// Get the hue from the "fixed" mode.
		
		int distance = 5;
		bool displayChanged = false;
		const int numRibbons = 12;
		
		int ribbons[][2] = { {0,15},	{16,35},	{36,50},	{51,65},	{66,81},	{82,101},	{102,116},	{117,131},	{132,142},	{143,151},	{152,160},	{161,168},	{169,179},	{180,188},	{189,197},	{198,205},	{206,218},	{219,230},	{231,240},	{241,254},	{255,266},	{267,285},	{286,296}};

			
		static int positions[numRibbons] = {0};
		if (firstRun) {
			for (int i = 0; i < numRibbons; i++) {
				positions[i]=random(ribbons[i][0], ribbons[i][1]);
			}
		}

		static int tailLength = 5; //getSetting(3,1);	
		tailLength = constrain(tailLength, 0, 10);
		const int saturationDecrement = (int) (255.0 / (float)tailLength);
		
		static int speed = 5; //getSetting(3,2);
		if (speed < 0 || speed > 250) speed = 5; 

		int updateTimeGap = 250 / speed;
		
		static int dir = 1;
		
		long dialOneNewPosition =  dialOne.read();
		if (firstRun) dialOneOldPosition = dialOneNewPosition;
		if (dialOneNewPosition % 4 == 0 && dialOneNewPosition != dialOneOldPosition) {
			int dir = (dialOneNewPosition - dialOneOldPosition) / 4;
			dialOneOldPosition = dialOneNewPosition;

			tailLength += dir;
			tailLength = constrain(tailLength, 0, 10);
			p("old = %u, new = %u, dir = %i, taillength = %u \n", dialOneOldPosition, dialOneNewPosition, dir, tailLength);
//			setSetting(3,1,tailLength);
			displayChanged = true;

		}

// 		long dialTwoNewPosition =  dialTwo.read();
// 		if (dialTwoNewPosition % 4 == 0 && dialTwoNewPosition != dialTwoOldPosition) {
// 			int dir = (dialTwoNewPosition - dialTwoOldPosition) / 4;
// 			dialTwoOldPosition = dialTwoNewPosition;
// 			speed += dir;
// 			speed = constrain(speed, 0, 10);
// //			setSetting(3,2,speed);
// 			displayChanged = true;
// 		}
		
// 		if (displayChanged || firstRun) {
// 			p("firstRun: %u, displayChanged: %u \n",firstRun, displayChanged);
// 			char string[16];
// 			sprintf(string, "lgth:%i, spd:%i", tailLength, speed);
// 			printLcd(1, string);
// 		}
		displayChanged = false;		

		if( (millis() - lastUpdate) > updateTimeGap) {
			memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
			for (int ribbonNumber = 0; ribbonNumber < numRibbons; ribbonNumber++) {
				int firstLed = ribbons[ribbonNumber][0];
				int lastLed = ribbons[ribbonNumber][1];
				int length = lastLed - firstLed;
				int position = positions[ribbonNumber];
				p("position = %u, length = %u, firstLed = %u , lastLed = %u \n", position, length, firstLed, lastLed);
				int sat = 255;
				for (int i = 0; i < tailLength; i++) {
					int led = position - i;
					if (led < 0) {
						led = lastLed + led;
					} else {
						led = led + firstLed;
					}
					
					//p("ribbon = %u, i = %u, led[%i] = %u \n", ribbonNumber, i, led, sat);
					leds[led] = CHSV( hue, sat ,  sat);
					sat = sat - saturationDecrement;
				}
				positions[ribbonNumber] = (position + 1) % length;
			}
			
			
			
			LEDS.show();
			lastUpdate = millis();
		}
		
	}
	
