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
int brightness = 0; 
boolean firstChange = true;
boolean firstRun = true;
int settings[NUM_MODES * SETTINGS_PER_MODE]; // settings array

volatile boolean changeModeFlag;


rgb_lcd lcd;
CRGB leds[NUM_LEDS];
Encoder dialMode(17, 11);
Encoder dialBrightness(10, 9);
Encoder dialOne(4, 3);
Encoder dialTwo(1, 23);



char* modeNames[]={
	"1 Fixed",
	"2 Chase",
	"3 Fade",
	"4 Rainbow",
	"5 Sparkles",
	"6 SparklesR"
};


void setup() {
	// set up the LCD's number of columns and rows:
	lcd.begin(16, 2);
		
	pinMode(MODE_PIN, INPUT_PULLUP);
	attachInterrupt(MODE_PIN, changeModeInterrupt, RISING);
	
	LEDS.addLeds<WS2811, 14>(leds, NUM_LEDS);
	
	brightness = getSetting(50,1);
	setBrightness(true);
	
	Serial.begin(9600);
	delay(1000);
}



void loop() {
	setBrightness(false);
	
	// The button has been pressed
	if (changeModeFlag) {
		currentMode = newMode;
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
	
	switch (currentMode) {
		case 0: fixed(firstRun); break;
		case 1: chasing(firstRun); break;
		//case 2: fade(); break;
		case 3: rainbow(firstRun); break;
		case 4: sparkles(firstRun, false); break;
		case 5: sparkles(firstRun, true); break;
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
		setSetting(50, 1, brightness);
		char string[16];
		sprintf(string, "%s %u", modeNames[currentMode], brightness);
		printLcd(0, string);
				
		LEDS.setBrightness(brightness);
		LEDS.show();
	}
}




