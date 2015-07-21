#ifndef _AUDIO_h
#define _AUDIO_h

#include "Arduino.h"
#include "utils.h"

#define FFT_SIZE 128

class LocalAudio
{
 private:
	static IntervalTimer samplingTimer;
	static int sampleCounter;
	static bool samplingDone; 
	static int sampleRate;
	static int inputPin;
	static void callback();	
	static int sampleSize;
	static volatile int  volume;

 public:
 	static int samples[];
	static void init ();
	static void start();
	static void stop();
	static int getVolume();
};



#endif

