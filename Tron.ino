#include "settings.h"

#include "mode_fixed.h"
#include "mode_rainbow.h"
#include "mode_sparkles.h"
#include "mode_falling.h"
#include "mode_rainbow.h"
#include "mode_chasing.h"

#include "utils.h"

#include <Wire.h>
#include <SD.h>
#include "rgb_lcd.h"
#include "FastSPI_LED2.h"
#include <Encoder.h>
#include <eeprom.h>


int mainPosition = 0;
long oldPosition  = 0;
int currentMode = 2 ;
int newMode = 0;
int displayModeTimeout = 2;
int displayModeChangeTime = millis();
int brightness = 0; 
boolean firstChange = true;
boolean firstRun = true;
int settings[NUM_MODES * SETTINGS_PER_MODE]; // settings array

volatile boolean changeModeFlag = false;


rgb_lcd lcd;
CRGB leds[NUM_LEDS];
Encoder dialMode(17, 11);
Encoder dialBrightness(4, 9);
Encoder dialOne(10, 3);
Encoder dialTwo(1, 23);

//PIN 4 AND 10

char* modeNames[]={
	"1 Fixed",
	"2 Chase",
	"3 Falling",
	"4 Rainbow",
	"5 Sparkles",
	"6 Spkls Rnbw",
	"7 Helmet"
};


void clearEeprom() {
	for (int i = 0; i< 2048; i++) {
		EEPROMWriteInt(i, 0);
	}
}

void setup() {
	
	// set up the LCD's number of columns and rows:
	lcd.begin(16, 2);
		
	pinMode(MODE_PIN, INPUT_PULLUP);
	
	
	attachInterrupt(MODE_PIN, changeModeInterrupt, RISING);
	
	LEDS.addLeds<WS2811, 7>(leds, 0, 66); // leg1
	LEDS.addLeds<WS2811, 8>(leds, 66, 66); //leg 2
 	LEDS.addLeds<WS2811, 20>(leds, 132, 37); // front left
 	LEDS.addLeds<WS2811, 14>(leds, 169, 37); // front right
 	LEDS.addLeds<WS2811, 21>(leds, 206, 35); //right arm
 	LEDS.addLeds<WS2811, 2>(leds, 241, 26); // left arm
 	LEDS.addLeds<WS2811, 5>(leds, 267, 31); //back
	

	if (digitalRead(MODE_PIN) == LOW) {
		clearEeprom();
	}
	
	brightness = getSetting(50,2);
	setBrightness(true);
	Serial.begin(9600);
	
	


	currentMode = getSetting(50,1);
	newMode = currentMode;
	
}



void loop() {

	
	setBrightness(false);
	
	
	// The button has been pressed
	if (changeModeFlag) {
		currentMode = newMode;
		p("changed mode to %u \n", currentMode);
		setSetting(50,1, currentMode);
		changeModeFlag = false;
		firstChange = true;
		firstRun = true;
		char string[16];
		sprintf(string, "%s %u", modeNames[currentMode], brightness);
		printLcd(0, string);
	}
	
	
	long newPosition = dialMode.read();

	
	if (newPosition % 4 == 0 && newPosition != oldPosition) {
		if (firstChange == true) {
			saveLcd();
			firstChange = false;
		}
		int dir = (newPosition - oldPosition) / 4; 
		oldPosition = newPosition;
		newMode += dir;
		if (newMode == NUM_MODES) newMode = NUM_MODES - 1;
		if (newMode == -1) newMode = 0;
		p("%i %i %i %u %s\n", newPosition, oldPosition, dir, newMode, modeNames[newMode]);
		lcd.clear();
		printLcd(0, modeNames[newMode]);
		displayModeChangeTime = millis();

	}
	
	if (millis() > displayModeChangeTime + 1000 * displayModeTimeout && currentMode != newMode) {
		newMode = currentMode;
		firstChange = true;
		revertLcd();
	}
	//
	switch (currentMode) {
		case 0: mode_fixed(firstRun); break;
		case 1: mode_chasing(firstRun); break;
		case 2: mode_falling(false); break;
		case 3: mode_rainbow(firstRun); break;
		case 4: mode_sparkles(firstRun, false); break;
		case 5: mode_sparkles(firstRun, true); break;
				//case 5: mode_helmet(firstRun); break;
	}
	firstRun = false;
}


void changeModeInterrupt()
{
	cli();
	changeModeFlag = true;
	sei();
}

void setBrightness(boolean firstRun) {
	static long oldPosition = 0;
	long newPosition =  dialBrightness.read();
	if (newPosition % 4 == 0 && newPosition != oldPosition || firstRun) {
		int dir = (newPosition - oldPosition) / 4; 
		oldPosition = newPosition;
		brightness += 5 * dir;
		if (brightness < 0) brightness = 0;
		if (brightness > 256) brightness = 256;
		setSetting(50, 2, brightness);
		char string[16];
		sprintf(string, "%s %u", modeNames[currentMode], brightness); 
		printLcd(0, string);
				
		LEDS.setBrightness(brightness);
		LEDS.show();
	}
	

}




