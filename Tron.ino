#include "modes.h"
#include "settings.h"


#include "utils.h"
#include <Wire.h>
#include <SD.h>
#include "rgb_lcd.h"
#include "FastSPI_LED2.h"
#include <Encoder.h>
#include <Audio.h>
#include <eeprom.h>


int mainPosition = 0;
long oldPosition  = 0;
int currentMode = 0 ;
int newMode = 0;
int displayModeTimeout = 2;
int displayModeChangeTime = millis();

int settings[NUM_MODES * SETTINGS_PER_MODE]; // settings array

volatile boolean changeModeFlag;
#define NUM_LEDS 115
Encoder modeDial(17, 11);

rgb_lcd lcd;
CRGB leds[NUM_LEDS];
Encoder dialOne(10, 9);
Encoder dialTwo(4, 3);
//Modes modes(&lcd, &leds, &dialOne, &dialTwo);

char* modeNames[]={
	"1 Fixed",
	"2 Chase",
	"3 Fade",
	"4 Rainbow"
};


void setup() {
	// set up the LCD's number of columns and rows:
	lcd.begin(16, 2);
	lcd.print(modeNames[0]);
	
	pinMode(MODE_PIN, INPUT_PULLUP);
	attachInterrupt(MODE_PIN, changeModeInterrupt, RISING);
	
	LEDS.setBrightness(16);
	LEDS.addLeds<WS2811, 14>(leds, NUM_LEDS);
	
	Serial.begin(9600);

	delay(1000);
}



void loop() {
	setBrightness();
	
	// The button has been pressed
	if (changeModeFlag) {
		currentMode = newMode;
		changeModeFlag = false;
		Serial.println(currentMode);
	}
	
	lcd.setCursor(0, 0);
	long newPosition = modeDial.read();

	
	if (newPosition % 4 == 0 && newPosition != oldPosition) {
		int dir = (newPosition - oldPosition) / 4; 
		oldPosition = newPosition;
		newMode += dir;
		if (newMode == NUM_MODES) newMode = NUM_MODES - 1;
		if (newMode == -1) newMode = 0;
		p("%i %i %i %u %s\n", newPosition, oldPosition, dir, newMode, modeNames[newMode]);
		lcd.clear();
		lcd.print(modeNames[newMode]);
		displayModeChangeTime = millis();
	}
	
	if (millis() > displayModeChangeTime + 1000 * displayModeTimeout && currentMode != newMode) {
		newMode = currentMode;
		lcd.clear();
		lcd.print(modeNames[currentMode]);
	}
	
	switch (currentMode) {
		case 0: fixed(dialOne, dialTwo); break;
		//case 1: chasing(); break;
		//case 2: fade(); break;
		//case 3: rainbow(); break;
	}
	
}


void changeModeInterrupt()
{
	cli();
	changeModeFlag = true;
	sei();
}

void setBrightness() {
	static long oldPosition = 16;
	static int brightness = 0;
	long newPosition =  dialTwo.read();
	if (newPosition % 4 == 0 && newPosition != oldPosition) {
		int dir = (newPosition - oldPosition) / 4; 
		oldPosition = newPosition;
		brightness += 5 * dir;
		if (brightness < 0) brightness = 0;
		if (brightness > 100) brightness = 100;
		LEDS.setBrightness(brightness);
		LEDS.show();
	}
}




