#include "utils.h"
//#include <Encoder.h>

// extern Encoder dialOne;
// extern Encoder dialTwo;

char currentLcdLines[2][16];
char savedLcdLines[2][16];


char* p(char *fmt, ... ){
	char tmp[128]; // resulting string limited to 128 chars
	va_list args;
	va_start (args, fmt );
	vsnprintf(tmp, 128, fmt, args);
	va_end (args);
	Serial.print(tmp);
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
	p("Setting %u to %u \n", (mode * SETTINGS_PER_MODE + setting) * 2, val);
	EEPROMWriteInt(((mode * SETTINGS_PER_MODE) + setting) * 2, val);
}

int getSetting(int mode, int setting) {
	return EEPROMReadInt(((mode * SETTINGS_PER_MODE) + setting) * 2);
}

void printLcd(int line, char *text) {
	lcd.setCursor( 0, line );
	lcd.print("                ");
	lcd.setCursor( 0, line );
	strcpy(currentLcdLines[line], text);
	lcd.print(text);
}

void saveLcd() {
	strcpy(savedLcdLines[0], currentLcdLines[0]);
	p("saved lcd line 0: %s \n", currentLcdLines[0] );
	strcpy(savedLcdLines[1], currentLcdLines[1]);
	p("saved lcd line 1: %s \n",  currentLcdLines[1] );
}

void revertLcd() {
	printLcd(0, savedLcdLines[0]);
	printLcd(1, savedLcdLines[1]);
}

// int getDialOne() {
// 	static int dir = 1;
// 	static long dialOneOldPosition = 0;
// 	long dialOneNewPosition = 0;
// 	int change = 0;
// 	
// 	dialOneNewPosition =  dialOne.read();
// 	//		if (firstRun) dialOneOldPosition = dialOneNewPosition;
// 	if (dialOneNewPosition != dialOneOldPosition && dialOneNewPosition % 4 == 0) {
// 		change = (dialOneNewPosition - dialOneOldPosition) / 4;
// 		dialOneOldPosition = dialOneNewPosition;
// 	}
// 	return change;
// }