#include "LocalAudio.h"

IntervalTimer LocalAudio::samplingTimer;

int LocalAudio::sampleCounter = 0;
bool LocalAudio::samplingDone;
int LocalAudio::sampleRate = 9000;
int LocalAudio::inputPin = A8;  // Input ADC pin for LocalAudio data.
int LocalAudio::samples[FFT_SIZE];
volatile int LocalAudio::volume;

void LocalAudio::init() {
	analogReadResolution(10); // Bits of resolution for the ADC.
	analogReadAveraging(16); // Number of samples to average with each ADC reading.
}

void LocalAudio::start() {
// 	sampleCounter = 0;
// 	samplingDone = false;
	samplingTimer.begin(callback, 1000000/sampleRate);
}

void LocalAudio::callback() {
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

void LocalAudio::stop() {
	samplingDone = true;
	samplingTimer.end();
	
}

int LocalAudio::getVolume() {
	return volume;
}


LocalAudio LocalAudio;

