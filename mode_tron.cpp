#include "settings.h"
#include "Arduino.h"
#include "rgb_lcd.h"
#include "FastSPI_LED2.h"
#include "utils.h"
#include "mode_tron.h"

extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];

void mode_tron(bool firstRun) {
	for(int i = 0; i < NUM_LEDS; i++) {
		leds[i].setHue(60);
	}
	LEDS.show();


}