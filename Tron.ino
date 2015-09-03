#include "settings.h"

// For some (currently) unknown reason, all the imports used in other files also
// need to be imported here as well.
// Otherwise the compiler throws: "fatal error: XXX.h: No such file or directory"
#include <Wire.h>
#include <rgb_lcd.h>
#include <FastLED.h>
#include <Encoder.h>
#include <EEPROM.h>
//#include <Audio.h>
//#include <Wire.h>
//#include <SPI.h>
//#include <SD.h>

#include "mode_fixed.h"
#include "mode_rainbow.h"
#include "mode_sparkles.h"
#include "mode_falling.h"
#include "mode_rainbow.h"
#include "mode_volume.h"
#include "mode_volume2.h"
#include "mode_fire.h"
#include "mode_count.h"
#include "mode_fill.h"
#include "LocalAudio.h"
#include "utils.h"





int displayModeTimeout = 2;
int displayModeChangeTime = millis();

// This is set to true by an interrupt when the "mode" button is pressed
boolean changeModeFlag = false;

boolean firstRun = true;


rgb_lcd lcd;
CRGB leds[NUM_LEDS];
Encoder dialMode(17, 11);
Encoder dialBrightness(4, 9);
Encoder dialOne(10, 3);
Encoder dialTwo(1, 23);


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
	LEDS.addLeds<WS2811, 7, GRB>(leds, 0, 66).setCorrection(TypicalLEDStrip); // leg1
	LEDS.addLeds<WS2811, 8, GRB>(leds, 66, 66).setCorrection(TypicalLEDStrip); //leg 2
	LEDS.addLeds<WS2811, 20, GRB>(leds, 132, 37).setCorrection(TypicalLEDStrip); // front left
	LEDS.addLeds<WS2811, 14, GRB>(leds, 169, 37).setCorrection(TypicalLEDStrip); // front right
	LEDS.addLeds<WS2811, 21, GRB>(leds, 206, 35).setCorrection(TypicalLEDStrip); //right arm
	LEDS.addLeds<WS2811, 2, GRB>(leds, 241, 26).setCorrection(TypicalLEDStrip); // left arm
	LEDS.addLeds<WS2811, 5, GRB>(leds, 267, 31).setCorrection(TypicalLEDStrip); //back
	

	// If we cock up the settings and save them to the EEProm in such a way the 
	// Teensy crashes on boot, this is the get-out-jail card!
	// Press and hold the mode dial btn and turn it on to reset to defaults.
	if (digitalRead(MODE_PIN) == LOW) {
		clearEeprom();
		setDefaults();
		while (digitalRead(MODE_PIN) == LOW) {}
	}
	
	setBrightness();
	if (DEBUG) {
		Serial.begin(9600);
	}

	// Need a small delay here otherwise the Teensy crashes on boot,
	// but only on the fixed mode for some reason.
	delay(100);
}


void loop() {
	
	readDials();
	
	// The button has been pressed
	if (changeModeFlag) {
		// Get the setting the mode dial is on
		// And set out main mode to that
		setSetting(MODE_MAIN, 0, getSetting(MODE_MAIN, 1));
		int modeNumber = getSetting(MODE_MAIN, 0);
		int settingNumber = getSetting(modeNumber, MODE_SETTING);
		p("modeNumber : %u, settingNumber: %u \n", modeNumber, settingNumber);
		firstRun = true;
		writeDisplay();
		// Reset brightness to normal in case a mode boosted it
		setBrightness();
		LocalAudio::stop();
		
		changeModeFlag = false;
	}
	
	// The mode dial has been changed
	if (getSettingChanged(MODE_MAIN,1)) {
		lcd.clear();
		printLcd(0, getModeName(getSetting(MODE_MAIN, 1)));
		displayModeChangeTime = millis();
	}
	
	if (getSettingChanged(MODE_MAIN, 2)) {
		setBrightness();
	}

	// Timeout:  If we have exceeded the timeout, and we are not already on the 
	// current mode, then reset the display
	if (
		millis() > displayModeChangeTime + 1000 * displayModeTimeout 
		&& getSetting(MODE_MAIN, 1) != getSetting(MODE_MAIN, 0)
	) {
		setSetting(MODE_MAIN, 1, getSetting(MODE_MAIN, 0));
		writeDisplay();
	}

	switch (getSetting(MODE_MAIN, 0)) {
		case 0: mode_fixed(firstRun); break;
		case 1: mode_falling(firstRun); break;
		case 2: mode_sparkles(firstRun, 0); break;
		case 3: mode_sparkles(firstRun, 1); break;
		case 4: mode_sparkles(firstRun, 2); break;
		case 5: mode_rainbow(firstRun); break;
		case 6: mode_fire(); break;
		case 7: mode_volume(firstRun); break;
		case 8: mode_volume2(firstRun); break;
		case 9: mode_fill(firstRun); break;
	}
	firstRun = false;
}


void changeModeInterrupt()
{
	cli();
	changeModeFlag = true;
	sei();
}

void setBrightness() {
	LEDS.setBrightness(getSetting(MODE_MAIN, 2));
	LEDS.show();
}






