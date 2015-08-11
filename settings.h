// settings.h

#ifndef _SETTINGS_h
#define _SETTINGS_h

#define DEBUG true
#define MAX_BRIGHTNESS 255
#define MODE_PIN 12
#define NUM_MODES 10
#define SETTINGS_PER_MODE 5
#define NUM_LEDS 297

#define MODE_SETTING 0

#define GLOBAL_SETTINGS 0
#define MODE_MAIN 0

#define MODE_FIXED 0
#define MODE_FALLING 1
#define MODE_SPARKLES 2
#define MODE_SPARKLES_CHNG 3
#define MODE_SPARKLES_RNBW 4
#define MODE_RAINBOW 5
#define MODE_FIRE 6
#define MODE_VOLUME 7
#define MODE_VOLUME2 8
#define MODE_COUNT 9

const int RIBBONS[][2] = {
	{ 0, 15 }, // upper right leg inside
	{ 16, 35 }, // upper right leg outside
	{ 36, 50 }, // lower right leg outside 
	{ 51, 65 },  // lower right leg inside
	{ 66, 81 },  // upper left leg inside
	{ 82, 101 },  // upper left leg outside
	{ 102, 116 },  // lower left leg outside
	{ 117, 131 },   // lower left leg inside
	{ 132, 142 }, // top left top
	{ 143, 151 }, // top left bottom
	{ 152, 160 },  // bottom left top
	{ 161, 168 }, // bottom left bottom
	{ 169, 179 },  // top right top
	{ 180, 188 },  // top right bottom
	{ 189, 197 },  // bottom right top
	{ 198, 205 },  // bottom right bottom
	{ 206, 218 },  // right arm top
	{ 219, 230 },  // right arm top
	{ 231, 240 },  // right arm bottom
	{ 241, 254 },  // left arm top 
	{ 255, 266 },  // left arm top
	{ 267, 285 },  // back outside
	{ 286, 296 }   // back inside
};


#endif

