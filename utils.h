// utils.h

#ifndef _UTILS_h
#define _UTILS_h

#include <Arduino.h>
#include <stdarg.h>
#include <eeprom.h>
#include <rgb_lcd.h>
#include <Encoder.h>

#include "settings.h";

extern rgb_lcd lcd;

void p(char *fmt, ... );
void EEPROMWriteInt(int p_address, int p_value);
unsigned int EEPROMReadInt(int p_address);
void EEPROMWriteArray(int data[NUM_MODES * SETTINGS_PER_MODE]);
void EEPROMReadArray(int *buf);
void setSetting(int mode, int setting, int val);
int getSetting(int mode, int setting);
void printLcd(int line, char *text);
void saveLcd();
void revertLcd();
int getDialOne();
int getDialTwo();
void readDials();
long readDial(Encoder *dial);
int getSettingChanged(int mode, int setting);
char* getSettingName(int mode, int setting);
void writeDisplay();
char* getModeName(int mode);
#endif

