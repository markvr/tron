#ifndef _AUDIO_h
#define _AUDIO_h

#include "Arduino.h"
#include "utils.h"

#define FFT_SIZE 256

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
	static volatile int samples[];
	static void init ();
	static void start();
	static void stop();
	static unsigned int getVolume();
};



#endif

