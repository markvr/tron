#include "modes.h"

void Modes::fixed() {
	static long oldPosition = 0;
	static int hue = 0;
	long newPosition =  dialOne->read();
	if (newPosition % 4 == 0 && newPosition != oldPosition) {
		int dir = (newPosition - oldPosition) / 4;
		oldPosition = newPosition;
		hue += 10* dir;
		if (hue < 0) hue = 0;
		if (hue > 250) hue = 250;
		//lcd->clear();
		//lcd->print(modeNames[currentMode]);
		//lcd->setCursor ( 0, 1 );
		char string[10];
		sprintf(string, "h%i", hue);
		lcd->print(string);

		for(int i = 0; i < NUM_LEDS; i++) {
			&leds[i].setHue(hue);
		}
		LEDS.show();
		

	}
}


void Modes::chasing() {
	static unsigned long lastUpdate = 0;
	static int level = 0;
	static int dir = 1;
	int updateTimeGap = 10;
	static int currentLED = 0;
	if( (millis() - lastUpdate) > updateTimeGap) {
		memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
		leds[currentLED] = CRGB(128, 0, 0);
		currentLED = (currentLED + dir) % NUM_LEDS;
		if (currentLED == 0 || currentLED == NUM_LEDS - 1) dir = dir * -1;
		
		lastUpdate = millis();
		LEDS.show();
	}

}

void Modes::fade() {
	static unsigned long lastUpdate = 0;
	int updateTimeGap = 5;
	static int level = 0;
	static int dir = 1;
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

void Modes::rainbow() {
	static int offset = 0;
	int updateTimeGap = 0;
	static unsigned long lastUpdate = 0;
	if( (millis() - lastUpdate) > updateTimeGap) {
		offset++;
		lastUpdate = millis();
		for (int i = 0; i < NUM_LEDS; i++) {
			leds[i].setHue(i + offset);
		}
		LEDS.show();
	}
}



