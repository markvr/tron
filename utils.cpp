#include "utils.h"
#include <Encoder.h>

extern Encoder dialMode;
extern Encoder dialBrightness;
extern Encoder dialOne;
extern Encoder dialTwo;

bool settingChanged[NUM_MODES + 1][SETTINGS_PER_MODE];
int settingsVals[1024];

char currentLcdLines[2][16];
char savedLcdLines[2][16];

/*
The "settings" is a 2D array of [modes]x[settings]
It is packed into a 1D in blocks of SETTINGS_PER_MODE
i.e. 
mode			0											1
value	[0..SETTINGS_PER_MODE][1*SETTINGS_PER_MODE + 1..1*SETTINGS_PER_MODE + SETTINGS_PER_MODE] etc

It's contents are as follows:
								settings
											0					1
global		currentMode					potentialMode		brightness
modes	0	currentDisplayedSetting		Setting 0 Value		Setting 1 Value
		1

mode=0 stores "global" settings which are:
	currentMode - the current mode we in
	potentialMode - the displayed mode if the mode dial has been changed
	brightness - well umm, the brightness!


*/

char* modeNames[] = {
	"1 Fixed",
	"2 Falling",
	"3 Rainbow",
	"4 Sparkles",
	"5 Spkls Rnbw",
	"6 Volume"
};

char* settingNames[NUM_MODES][SETTINGS_PER_MODE] = {
	{ "1 hue", "2 saturation", "", "" },				// Fixed
	{ "1 length", "2 speed", "3 hue", "4 saturation" },		// Falling
	{ "1 number", "2 speed", "3 hue", "4 saturation" },		// Sparkles
	{ "1 number", "2 speed", "", "" },					// Sparkles Rainbow
	{ "1 density", "2 speed", "", "" }					// Rainbow
};

// How many above settings are needed for each mode?
int settingCounts[] = {
	2,
	4,
	4,
	2,
	2
};

// Define the settings that apply to each setting :)
// This is [min value, max value, loop around]
int settingSettings[NUM_MODES][SETTINGS_PER_MODE][3] = {
	{ { 0, 255, 1 }, { 0, 255, 0 }, {}, {} },
	{ { 0, 10, 0 }, { 0, 10, 0 }, { 0, 255, 1 }, { 0, 255, 0 } },
	{ { 0, 30, 0 }, { 0, 10, 0 }, { 0, 255, 1 }, { 0, 255, 0 } },
	{ { 0, 30, 0 }, { 0, 10, 0 }, {}, {} },
	{ { 0, 10, 0 }, { 0, 10, 0 }, {}, {} }
};



void p(char *fmt, ... ){
	if (DEBUG) {
		char tmp[128]; // resulting string limited to 128 chars
		va_list args;
		va_start(args, fmt);
		vsnprintf(tmp, 128, fmt, args);
		va_end(args);
		Serial.print(tmp);
	}
}

void EEPROMWriteInt(int p_address, int p_value)
{
	byte lowByte = ((p_value >> 0) & 0xFF);
	byte highByte = ((p_value >> 8) & 0xFF);

	EEPROM.write(p_address, lowByte);
	EEPROM.write(p_address + 1, highByte);
}

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int EEPROMReadInt(int p_address)
{
	byte lowByte = EEPROM.read(p_address);
	byte highByte = EEPROM.read(p_address + 1);

	return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}


void setSetting(int mode, int setting, int val) {
	p("Setting mode %u, setting % u to %u \n", mode, setting, val);
	//EEPROMWriteInt(((mode * SETTINGS_PER_MODE) + setting) * 2, val);
	settingsVals[(mode * SETTINGS_PER_MODE) + setting] = val;
}

int getSetting(int mode, int setting) {
	//return EEPROMReadInt(((mode * SETTINGS_PER_MODE) + setting) * 2);
	return settingsVals[(mode * SETTINGS_PER_MODE) + setting];
}

int getSettingChanged(int mode, int setting) {
	bool value = settingChanged[mode][setting];
	settingChanged[mode][setting] = false;
	return value;
}

char* getSettingName(int mode, int setting) {
	return settingNames[mode][setting];
}

char* getModeName(int num) {
	return modeNames[num];
}


void writeDisplay() {
	int mode = getSetting(MODE_MAIN, 0);
	int brightness = getSetting(MODE_MAIN, 2);
	int setting = getSetting(mode + 1, MODE_SETTING);
	int settingValue = getSetting(mode  + 1, setting + 1);
	p("%u %u %u %u\n", mode, brightness, setting, settingValue);
	char string[16];
	sprintf(string, "%-13s%3u", modeNames[mode], brightness);
	printLcd(0, string);
	sprintf(string, "%-13s%3u", getSettingName(mode, setting), settingValue);
	printLcd(1, string);
}

void readDials() {
	// Mode
	int position =  readDial(&dialMode);
	if (position != 0) {
		p("position: %u\n", position);
		settingChanged[MODE_MAIN][1] = true;
		setSetting(MODE_MAIN, 1, constrain(getSetting(MODE_MAIN, 1) + position, 0, NUM_MODES));
	}

	// Brightness
	position = readDial(&dialBrightness);
	if (position != 0) {
		p("position: %u\n", position);
		settingChanged[MODE_MAIN][2] = true;
		setSetting(MODE_MAIN, 2, constrain(getSetting(MODE_MAIN, 2) + 10 * position, 0, MAX_BRIGHTNESS));
		writeDisplay();
	}

	// Setting Type
	position = readDial(&dialOne);
	if (position != 0) {
		int currentMode = getSetting(MODE_MAIN, 0);
		// We set/get the current value for which setting is being displayed
		// for each mode in MODE_SETTING (=0) on the modes settings
		int newSetting = getSetting(currentMode + 1, MODE_SETTING) + position;
		newSetting = constrain(newSetting, 0, settingCounts[currentMode] - 1);
		p("mode: %u, newSetting %d, \n", currentMode, newSetting );

		setSetting(currentMode + 1, MODE_SETTING, newSetting);
		writeDisplay();
	}

	//Setting Value
	position = readDial(&dialTwo);
	if (position != 0) {
		int mode = getSetting(MODE_MAIN, 0);
		int settingNumber = getSetting(mode + 1, MODE_SETTING);
		int min = settingSettings[mode][settingNumber][0];
		int max = settingSettings[mode][settingNumber][1];
		int loop = settingSettings[mode][settingNumber][2];
		int newValue = getSetting(mode + 1, settingNumber + 1) + position;

		if (loop == 1) {
			if (newValue < min) newValue = max;
			if (newValue > max) newValue = min;
		} else {
			newValue = constrain(newValue, min, max);
		}
		// +1 because setting 0 is stored in position 1 in the array
		// (position 0 is the setting being currently displayed)
		setSetting(mode + 1, settingNumber + 1, newValue);
		writeDisplay();
	}
}

long readDial(Encoder *dial) {
	static long value = 0;
	long position = dial->read();

	if (position % 4 == 0 && position != 0) {
		position = position / 4;
		// give the encoders some time to stop "bouncing" before resetting
		delay(10); 
		// The dial has finished moving so reset it back to 0
		dial->write(0);
	} else {
		position = 0;
	}
	return position;
}

void printLcd(int line, char *text) {
	lcd.setCursor( 0, line );
	lcd.print("                ");
	lcd.setCursor( 0, line );
	lcd.print(text);
}
	