#include "Audio.h"
#include <math.h>

#define FFT_SIZE  64

IntervalTimer Audio::samplingTimer;
int Audio::sampleCounter;
bool Audio::samplingDone;
int Audio::fftSize = 64; // Size of the FFT.
int Audio::sampleRate = 9000;
int Audio::inputPin = A8;  // Input ADC pin for audio data.
bool Audio::newData;
int Audio::samples[FFT_SIZE];
int  Audio::spectrum[FFT_SIZE / 2];

arm_cfft_radix4_instance_f32 Audio::fft_inst;
// 
 const float DECAY_RATE = 0.93f;
 const int UPDATE_FREQUENCY = 20; //Hz -
 const int NUM_COLS = 5;
 const int COL_HEIGHT = 8;
 const int MAX_FFT_BIN_VALUE = 15000; // This is the approx max bin value seen in real life.
 //const int FFT_SIZE = 64;     // must match value in Audio.h
// 
// // These values were derived from trial and error recording audio and averageing the bin values out to
// // get something that looked vaguley even across the consolidation.
 int Audio::columnBounds[5][2] = { {1, 3}, {4, 6}, {7, 9}, {10, 12}, {13, 32}  };
// int scaleFactors[] = {5,4,6,2,15};
// int noise[] = {0, 152,	93,	106, 69, 64, 82, 58, 48, 71, 44, 42, 56, 40, 33, 47, 32, 29, 37, 30, 22, 32, 24, 22, 25, 24, 19, 24, 20, 20, 20, 21};
int Audio::volumeNoise = 25;
// 
// 
// // Column LED numbers - {bottom, top}
// int strips[][2] = {{82, 77}, {83, 89}, {97, 90}, { 98, 105}, {40,47}, {48, 55}, {63, 56}, {64, 70}, {76, 71} };
// 
// // Volume Averaging
// // We want to store the approx average of x seconds to scale the values of the columns
// // Testing shows that we run at approx 100 frames / sec.  This is too many to store all the values, so store framesASec
// const int numSecs = 5;		// Change these
// const int framesASec = 10;
// 
// // Don't change below
// int lastFrameTime = 0;
// int frameDelay = 1000 / framesASec;
// int currentFrame = 0;
// int numFrames = numSecs * framesASec;
// int frames[numSecs * framesASec][NUM_COLS];
// int volumeFrames[numSecs * framesASec];
// int prevScaledVolume = 0;
// int colours[8];




void Audio::init() {
	analogReadResolution(10); // Bits of resolution for the ADC.
	analogReadAveraging(32); // Number of samples to average with each ADC reading.
	arm_cfft_radix4_init_f32(&Audio::fft_inst, FFT_SIZE, 0, 1);
}

void Audio::start() {
	sampleCounter = 0;
	samplingDone = false;
//	p("starting audio\n");
	samplingTimer.begin(Audio::callback, 1000000/sampleRate);
}

void Audio::callback() {
	samples[sampleCounter] = analogRead(inputPin);
	sampleCounter++;
	if (sampleCounter == FFT_SIZE) {
		stop();
	}
}

void Audio::stop() {
	samplingDone = true;
	samplingTimer.end();
}

bool Audio::hasNewData() {
	return samplingDone;
}


void Audio::doFFT() {
	float samplesAsFloat[2*FFT_SIZE];
	for (int i = 0; i < FFT_SIZE; i++) {
		samplesAsFloat[2*i] = (float32_t) samples[i];
		samplesAsFloat[2*i + 1] = 0.0;
	} 
	arm_cfft_radix4_f32(&fft_inst, samplesAsFloat);

	// Calculate magnitude of complex numbers output by the FFT.
	float spectrumAsFloat[FFT_SIZE];
	arm_cmplx_mag_f32(samplesAsFloat, spectrumAsFloat, FFT_SIZE);
	for (int i = 0; i < FFT_SIZE/2; i++) {
		spectrum[i] = (int)spectrumAsFloat[i];
	}
}

void Audio::getSpectrum(int spectrumOut[]) {
	doFFT();
	memcpy(spectrumOut, spectrum, FFT_SIZE / 2 * sizeof(int));  // put current frame into frames[]
}

void Audio::getAdjustedSpectrum(int spectrumOut[]) {
	
}


void Audio::getSamples(int samplesOut[]	) {
	memcpy(samplesOut, samples, FFT_SIZE * sizeof(int));  // put current frame into frames[]
}

int Audio::getVolume() {
	// Get the RMS value - it's between approx 10 and 200
	float volume = 0;
	for (int i = 0; i < FFT_SIZE; i++) {
		volume += ( (samples[i] - 512) * (samples[i] - 512));
	}
	volume = sqrt(volume / FFT_SIZE);
	return (int) volume;
}

void Audio::sendInit() {
	Serial.write(0xFF);
}

void Audio::sendData(int data[], int length) {
	for (int i = 0; i < length; i++) {
		int val =  data[i];
		Serial.write( (val >> 8) & 0xff);
		Serial.write( val & 0xff);
	}
}

void Audio::printData(int data[], int length) {
	for (int i = 0; i < length; i++) {
		Serial.print(data[i]);
		Serial.print(",");
	}
	Serial.println();
}


void Audio::sendAudioData() {
	sendInit();
	sendData(samples, FFT_SIZE);
	sendData(spectrum, FFT_SIZE / 2);
}



Audio Audio;

