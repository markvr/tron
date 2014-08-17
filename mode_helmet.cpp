#include "settings.h"
#include "Arduino.h"
#include "rgb_lcd.h"
#include "FastSPI_LED2.h"
#include <Encoder.h>
#include "utils.h"
#include "Audio.h"

extern Encoder dialOne;
extern Encoder dialTwo;
extern Encoder dialBrightness;
extern rgb_lcd lcd;
extern CRGB leds[NUM_LEDS];

const float DECAY_RATE = 0.93f;
const int UPDATE_FREQUENCY = 20; //Hz -
const int NUM_COLS = 5;
const int COL_HEIGHT = 8;
const int MAX_FFT_BIN_VALUE = 15000; // This is the approx max bin value seen in real life.
const int FFT_SIZE = 32;     // must match value in Audio.h

// These values were derived from trial and error recording audio and averageing the bin values out to
// get something that looked vaguley even across the consolidation.
int columnBounds[5][2] = { {1, 3}, {4, 6}, {7, 9}, {10, 12}, {13, 32}  };
int scaleFactors[] = {5,4,6,2,15};
int noise[] = {0, 152,	93,	106, 69, 64, 82, 58, 48, 71, 44, 42, 56, 40, 33, 47, 32, 29, 37, 30, 22, 32, 24, 22, 25, 24, 19, 24, 20, 20, 20, 21};
int volumeNoise = 25;


// Column LED numbers - {bottom, top}
int strips[][2] = {{82, 77}, {83, 89}, {97, 90}, { 98, 105}, {40,47}, {48, 55}, {63, 56}, {64, 70}, {76, 71} };

// Volume Averaging
// We want to store the approx average of x seconds to scale the values of the columns
// Testing shows that we run at approx 100 frames / sec.  This is too many to store all the values, so store framesASec
const int numSecs = 10;		// Change these
const int framesASec = 10;

// Don't change below
int lastFrameTime = 0;
int frameDelay = 1000 / framesASec;
int currentFrame = 0;
int numFrames = numSecs * framesASec;
int frames[numSecs * framesASec][NUM_COLS];
int volumeFrames[numSecs * framesASec];
int prevScaledVolume = 0;
int colours[8];

// Holds the previous frames scaled values.  This differs to frames[][] as that is unscaled.
// This is used to make the column heights fall gracefully;
int prevScaledFrame[NUM_COLS] = {0};

int currentTime = 0;

void setStrip(int stripNum, int height) {
	//p("Setting %u to %u\n", stripNum, height);
	if (height > 8) height = 8;

	int top = strips[stripNum][0];
	int bottom = strips[stripNum][1];
	for (int i = 0; i <= abs(top-bottom); i ++) {
		if (top > bottom) {
			if (i <= height) {
				//leds.setPixel(bottom + i, colours[i]);
				p("a %u leds[%u]=%u\n",i, bottom+i, colours[i]);
				leds[bottom + i].setHue(colours[i]);
			} else {
				// turn off
				p("leds[%u]=%u\n",bottom+i, 0);
				leds[bottom + i] = 0;
			}
			
		} else {
			if (i <= height) {
				//leds.setPixel(bottom - i, colours[i]);
				p("b %u leds[%u]=%u\n",i, bottom-i, colours[i]);
				leds[bottom - i].setHue(colours[i]);
			} else {
				//leds.setPixel(bottom - i, 0);
				p("leds[%u]=%u\n",bottom-i, 0);
				leds[bottom - i] = 0;
			}
		}
	}

}

void setTopStrip(int val) {
	p("top: %u \n", val);
	for (int i = 0; i < 39; i++) {
	//leds.setPixel(i,0);
		leds[i] = 0;
	}
	
	for (int i = 0; i < val ; i++) {
		//leds.setPixel(20 + i, 0, 0, 20);
		leds[19 + i].setHue(200);
		//leds.setPixel(20 - i, 0, 0, 20);
		leds[19 - i].setHue(200);
	}
}

void  mode_helmet(bool first_run) {
	int green=0;
	int yellow = 60;
	int red = 100;
	
	if (first_run) {

		colours[0] = green;
		colours[1] = green;
		colours[2] = green;
		colours[3] = green;
		colours[4] = yellow;
		colours[5] = yellow;
		colours[6] = red;
		colours[7] = red;
		
		Audio::init();
		Audio::start();
	}
	if (Audio::hasNewData()) {

		int samples[FFT_SIZE];
		int spectrum[FFT_SIZE / 2];
		Audio::getSamples(samples);

		Audio::getSpectrum(spectrum);

				
		int volume = constrain(Audio::getVolume() - volumeNoise,0, 1000);
	
		for (int i = 0; i < FFT_SIZE / 2; i++) {
			spectrum[i] = constrain(spectrum[i] - noise[i], 0, 10000);
		}

		//Audio::sendInit();
		//Audio::sendData(samples, FFT_SIZE);
		//Audio::sendData(spectrum, FFT_SIZE / 2);
//

		int frame[NUM_COLS];
		for (int col = 0; col < NUM_COLS; col++) {
			frame[col] = 0;
			for (int bin = columnBounds[col][0]; bin <= columnBounds[col][1]; bin++) {
				frame[col] +=  spectrum[bin];
			}
			frame[col] /=  scaleFactors[col];
		}
	
		if (millis() > lastFrameTime + frameDelay) {
			memcpy(&frames[currentFrame], &frame, sizeof frame);  // put current frame into frames[]
			volumeFrames[currentFrame] = volume;
			currentFrame = (currentFrame + 1) % numFrames ;
		}

		int averageMin, averageMax;
		for (int i = 0; i < numFrames; i++) {
			int min = 99999, max = 0;
			for (int j = 0; j < NUM_COLS; j++) {
				if (frames[i][j] < min) min = frames[i][j];
				if (frames[i][j] > max) max = frames[i][j];
			}
			averageMin += min;
			averageMax += max;
		}

		averageMin /= numFrames;
		averageMax /= numFrames;

		// If it's quiet, the scaleFactor is too low and then noise dominates and everything just flickers.
		// So we constrain it to a min value chosen by trial and error.
		float scaleFactor = constrain(averageMax - averageMin, 300, 10000);
	
		int heights[NUM_COLS];
	
		for (int i = 0; i < NUM_COLS; i++) {
			// We want the values to "fall" back down, rather than be too jumpy, so we rise immediately,
			// but "fall" slowly. i.e. if we are < previous frame value, take that and reduce it a bit.
			// TODO: Rework this so we measure frames/sec and specify DECAY_RATE in frames/sec rather than a fixed constant

	
			
			if (frame[i] <= prevScaledFrame[i]) {
				frame[i] = prevScaledFrame[i] * DECAY_RATE;
			}
			prevScaledFrame[i] = frame[i]; // Save the new value for the next round.
	
			
			// Scale the columns, and constrain them to between 0 and the height of the col)
			heights[i] = floor(constrain((float)(frame[i] - averageMin) / scaleFactor * COL_HEIGHT, 0, COL_HEIGHT + 1));

			setStrip(i, heights[i]);
			if ( 8 - i >= 5) {
				setStrip(8 - i, heights[i]);
			}
			
		
		}
	
		int minVolume = 99999;
		int maxVolume = 0;
		for (int i = 0; i < numFrames; i++) {
			if (volumeFrames[i] > maxVolume) maxVolume = volumeFrames[i];
			if (volumeFrames[i] < minVolume) minVolume = volumeFrames[i];
		}
	
		if (volume < prevScaledVolume) volume = prevScaledVolume * DECAY_RATE;
		prevScaledVolume = volume;
	
		float volumeScaleFactor = constrain(maxVolume - minVolume, 0, 10000);
		int length = constrain( (volume - minVolume) / volumeScaleFactor * 20, 0, 38);

		setTopStrip(length);
	
		//leds.show();
		//setStrip(3,4);
		LEDS.show();
	
		Audio::start();
	}
}





