// utils.h

#ifndef _UTILS_h
#define _UTILS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif
#include "settings.h"
#include <stdarg.h>
#include <eeprom.h>

class UtilsClass
{
 protected:


 public:
	void init();
};

extern UtilsClass Utils;


char* p(char *fmt, ... );

void EEPROMWriteInt(int p_address, int p_value);
unsigned int EEPROMReadInt(int p_address);
void EEPROMWriteArray(int data[NUM_MODES * SETTINGS_PER_MODE]);
void EEPROMReadArray(int *buf);
void setSetting(int mode, int setting, int val);
int getSetting(int mode, int setting);



#endif

