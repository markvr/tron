#ifndef _AUDIO_h
#define _AUDIO_h

#include "Arduino.h"
#include "utils.h"
#define ARM_MATH_CM4

#include <arm_math.h>


class Audio
{
 private:
	static IntervalTimer samplingTimer;
	static int sampleCounter;
	static bool samplingDone; 
	static int fftSize;
	static int sampleRate;
	static int inputPin;
	static bool newData;
	static int volumeNoise;
	static int columnBounds[5][2];
	static arm_cfft_radix4_instance_f32 fft_inst;
	static void callback();	
	static void doFFT();

 public:
 	static int samples[];
	static int spectrum[];
	static void init ();
	static void start();
	static void stop();
	static bool hasNewData();
	static void getSamples(int samplesOut[]);
	static void getSpectrum(int samplesOut[]);
	static void getAdjustedSpectrum(int samplesOut[]);
	static int getVolume();
	static void sendData(int data[], int length);
	static void sendInit();
	static void sendAudioData();
	static void printData(int data[], int length) ;
};

extern Audio AUDIO;

#endif

