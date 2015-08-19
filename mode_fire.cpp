#include <Arduino.h>
#include <rgb_lcd.h>
#include <FastLED.h>

#include "settings.h"
#include "utils.h"
#include "mode_fire.h"

extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];

void mode_fire() {
	int sparking = getModeSetting(MODE_FIRE, 0) * 10;
	int cooling = getModeSetting(MODE_FIRE, 1) * 5;
	int speed = 10 - getModeSetting(MODE_FIRE, 2);
  
  static long lastUpdate = millis();
//	random16_add_entropy(random());
	const int ribbons[][2] = { 
		{ 0, 15 }, // upper right leg inside
		{ 16, 35 }, // upper right leg outside
		{ 36, 50 }, // lower right leg outside 
		{ 51, 65 },  // lower right leg inside
		{ 66, 81 },  // upper left leg inside
		{ 82, 101 },  // upper left leg outside
		{ 102, 116 },  // ?? helmet ??
		{ 117, 131 },   // ?? helmet??
		{ 132, 142 }, // top left top
		{ 143, 151 }, // top left bottom
		{ 152, 160 },  // bottom left top
		{ 161, 168 }, // bottom left bottom
		{ 169, 179 },  // top right top
		{ 180, 188 },  // top right bottom
		{ 189, 197 },  // bottom right top
		{ 198, 205 },  // bottom right bottom
		{ 206, 218 },  // right arm top
		{ 219, 230 },  // right arm top
		{ 231, 240 },  // right arm bottom
		{ 241, 254 },  // left arm top 
		{ 255, 266 },  // left arm top
		{ 267, 285 },  // back outside
		{ 286, 296 }   // back inside
	};
//	const int ribbons[][2] = { { 267, 285 } };
	const int numRibbons = 23;
  static int heat[numRibbons][30];
  
  if( (millis() - lastUpdate) > (10 * speed)) {
  	for (int ribbonNumber = 0; ribbonNumber < numRibbons; ribbonNumber++) {
  		int length = ribbons[ribbonNumber][1] - ribbons[ribbonNumber][0];

  		fire(heat[ribbonNumber], length, sparking, cooling); // run simulation frame
  
  		// Step 4.  Map from heat cells to LED colors
  		for (int j = length; j >= 0; j--) {
  			leds[ribbons[ribbonNumber][1] - j] = HeatColor(heat[ribbonNumber][j]);
  		}
  	}
  	LEDS.show(); // display this frame
    lastUpdate = millis();
  }

	

}



void fire(int heat[], int length, int sparking, int cooling) {
	// Array of temperature readings at each simulation cell


	// Step 1.  Cool down every cell a little
	for (int i = 0; i < length; i++) {
		heat[i] = qsub8(heat[i], random8(0, ((cooling * 10) / length) + 2));
	}

	// Step 2.  Heat from each cell drifts 'up' and diffuses a little
	for (int k = length - 1; k >= 2; k--) {
		heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
	}

	// Step 3.  Randomly ignite new 'sparks' of heat near the bottom
	if (random8() < sparking) {
		int y = random8(7);
		heat[y] = qadd8(heat[y], random8(160, 255));
	}

}
