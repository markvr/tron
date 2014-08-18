#include "Audio.h"

IntervalTimer Audio::samplingTimer;

int Audio::sampleCounter = 0;
bool Audio::samplingDone;
int Audio::sampleRate = 9000;
int Audio::inputPin = A8;  // Input ADC pin for audio data.
int Audio::samples[FFT_SIZE];
volatile int Audio::volume;

void Audio::init() {
	analogReadResolution(10); // Bits of resolution for the ADC.
	analogReadAveraging(16); // Number of samples to average with each ADC reading.
}

void Audio::start() {
// 	sampleCounter = 0;
// 	samplingDone = false;
	samplingTimer.begin(callback, 1000000/sampleRate);
}

void Audio::callback() {
	samples[sampleCounter] = analogRead(inputPin);
	sampleCounter++;
	if (sampleCounter == FFT_SIZE) {
		// Get the RMS value - it's between approx 10 and 200
		float floatVolume = 0;
		for (int i = 0; i < FFT_SIZE; i++) {
			floatVolume += ( (samples[i] - 512) * (samples[i] - 512));
		}
		volume = (int)sqrt(floatVolume / FFT_SIZE);
		sampleCounter = 0;
	}
}

void Audio::stop() {
	samplingDone = true;
	samplingTimer.end();
	
}

int Audio::getVolume() {
	return volume;
}


Audio Audio;

