#include "LocalAudio.h"

IntervalTimer LocalAudio::samplingTimer;

#define NUM_AVERAGE_FRAMES 60

int LocalAudio::sampleCounter = 0;
bool LocalAudio::samplingDone;
int LocalAudio::sampleRate = 10000;
int LocalAudio::inputPin = A8;  // Input ADC pin for LocalAudio data.
static int frameCount = 0;
static int frameCountMillis = millis();
static int frameAverages[NUM_AVERAGE_FRAMES];

volatile int LocalAudio::samples[FFT_SIZE];

void LocalAudio::init() {
	analogReadResolution(10); // Bits of resolution for the ADC.
	analogReadAveraging(4); // Number of samples to average with each ADC reading.
}

void LocalAudio::start() {
// 	sampleCounter = 0;
// 	samplingDone = false;
	samplingTimer.begin(callback, 1000000/sampleRate);
}

void LocalAudio::callback() {
	cli();
	samples[sampleCounter] = analogRead(inputPin);
	sampleCounter++;
	if (sampleCounter == FFT_SIZE) {
		sampleCounter = 0;
	}
	sei();
}

void LocalAudio::stop() {
	samplingDone = true;
	samplingTimer.end();
	
}

unsigned int LocalAudio::getVolume(bool scaled) {
	unsigned int volume;
	// Get the RMS value - it's between approx 10 and 200
	float floatVolume = 0;
	for (int i = 0; i < FFT_SIZE; i++) {
	//	p("%u ", samples[i]);
		floatVolume += ((samples[i] - 512) * (samples[i] - 512));
	}
	volume = (int)sqrt(floatVolume / FFT_SIZE);
	volume = volume - 5;	// background noise seems to hover around 5, so remove this
	if (volume < 0) volume = 0;
	frameCount++;
	if (frameCount == NUM_AVERAGE_FRAMES) {
		frameCount = 0;
	}
	frameAverages[frameCount] = volume;
	int min = 512, max = 0, scaledVolume = 0;
	int maxOriginal = 0;
	if (scaled) {
		for (int i = 0; i < NUM_AVERAGE_FRAMES; i++) {
			if (frameAverages[i] < min) min = frameAverages[i];
			if (frameAverages[i] > max) max = frameAverages[i];
		}
		maxOriginal = max;
		// If the max and min are to close the sensitivty is too high and noise
		// dominates. 
		if ((max - min) < 64) max = min + 64;		
		scaledVolume = 256 * (volume - min) / (max - min);
	}


	if (millis() - frameCountMillis > 1000) {
		frameCountMillis = millis();
		p("samples per second %i, vol %u, scaled %u, min %u, maxOriginal %u, max %u \n", frameCount, volume, scaledVolume, min, maxOriginal, max);
		frameCount = 0;
	}

	//Serial.println();
	//Serial.println(floatVolume);
	//Serial.println(volume);
//	p("\n RMS %.3f int %u \n", floatVolume, volume);
	if (scaled) {
		return scaledVolume;
	} else {
		return volume;
	}
}


LocalAudio LocalAudio;

