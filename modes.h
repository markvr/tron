// modes.h

#ifndef _MODES_h
#define _MODES_h

#include "Arduino.h"
#include "rgb_lcd.h"
#include "FastSPI_LED2.h"
#include <Encoder.h>
#define NUM_LEDS 20

class Modes {
 protected:

	rgb_lcd *lcd;
	CRGB *leds;
	Encoder *dialOne;
	Encoder *dialTwo;

 public:
	Modes(rgb_lcd *lcd, CRGB *leds, Encoder *dialOne, Encoder *dialTwo) : lcd(lcd), leds(leds), dialOne(dialOne), dialTwo(dialTwo) {};	
	void fixed();
	void fade();
	void rainbow();
	void chasing();

};


#endif

