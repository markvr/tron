#include "utils.h"

void UtilsClass::init()
{


}


UtilsClass Utils;


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

//void EEPROMWriteArray(int data[NUM_MODES * SETTINGS_PER_MODE]) {
	//for(int i = 0; i < NUM_MODES * SETTINGS_PER_MODE; i++) {
		//EEPROMWriteInt(2*i, data[i]);
	//}
//}
//
//void EEPROMReadArray(int *buf) {
	//for(int i = 0; i < NUM_MODES * SETTINGS_PER_MODE; i++) {
		//buf[i] = EEPROMReadInt(2*i);
	//}
//}

void setSetting(int mode, int setting, int val) {
	EEPROMWriteInt(((mode * SETTINGS_PER_MODE) + setting) * 2, val);
}

int getSetting(int mode, int setting) {
	return EEPROMReadInt(((mode * SETTINGS_PER_MODE) + setting) * 2);
}
