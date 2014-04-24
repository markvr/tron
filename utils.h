// utils.h

#ifndef _UTILS_h
#define _UTILS_h

#include "Arduino.h"
#include "settings.h"
#include <stdarg.h>
#include <eeprom.h>
#include "rgb_lcd.h"

extern rgb_lcd lcd;


char* p(char *fmt, ... );
void EEPROMWriteInt(int p_address, int p_value);
unsigned int EEPROMReadInt(int p_address);
void EEPROMWriteArray(int data[NUM_MODES * SETTINGS_PER_MODE]);
void EEPROMReadArray(int *buf);
void setSetting(int mode, int setting, int val);
int getSetting(int mode, int setting);
void printLcd(int line, char *text);
void saveLcd();
void revertLcd();

#endif

