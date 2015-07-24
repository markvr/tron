#include <Arduino.h>
#include <rgb_lcd.h>
#include <FastLED.h>

#include "settings.h"
#include "utils.h"
#include "mode_fire.h"

extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];

void mode_fire() {
//	random16_add_entropy(random());
	const int ribbons[][2] = { { 0, 15 }, { 16, 35 }, { 36, 50 }, { 51, 65 }, { 66, 81 }, { 82, 101 }, { 102, 116 }, { 117, 131 }, { 132, 142 }, { 143, 151 }, { 152, 160 }, { 161, 168 }, { 169, 179 }, { 180, 188 }, { 189, 197 }, { 198, 205 }, { 206, 218 }, { 219, 230 }, { 231, 240 }, { 241, 254 }, { 255, 266 }, { 267, 285 }, { 286, 296 } };
//	const int ribbons[][2] = { { 267, 285 } };
	const int numRibbons = 23;
	static int heat[numRibbons][20];
	for (int ribbonNumber = 0; ribbonNumber < numRibbons; ribbonNumber++) {
		int length = ribbons[ribbonNumber][1] - ribbons[ribbonNumber][0];
		
		fire(heat[ribbonNumber], length); // run simulation frame

		// Step 4.  Map from heat cells to LED colors
		for (int j = 0; j < length; j++) {
			leds[ribbons[ribbonNumber][0] + j] = HeatColor(heat[ribbonNumber][j]);
		}
		LEDS.show(); // display this frame
		//FastLED.delay(1000 / 100);
	}

}



// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120


void fire(int heat[], int length) {
	// Array of temperature readings at each simulation cell


	// Step 1.  Cool down every cell a little
	for (int i = 0; i < length; i++) {
		heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / length) + 2));
	}

	// Step 2.  Heat from each cell drifts 'up' and diffuses a little
	for (int k = length - 1; k >= 2; k--) {
		heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
	}

	// Step 3.  Randomly ignite new 'sparks' of heat near the bottom
	if (random8() < SPARKING) {
		int y = random8(7);
		heat[y] = qadd8(heat[y], random8(160, 255));
	}

}