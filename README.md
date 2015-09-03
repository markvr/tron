This is code to control a "Tron" style outfit that uses digital addressable LED ribbons to look awesome at festivals and parties!

This is written for a Teensy (https://www.pjrc.com/teensy/teensy31.html), which I can highly recommend over using Arduinos.

The wiring, circuit boards diagrams and parts needed are in the "tron-circuits" repo.

There are currently approx 10 different modes and animations, ranging from ambient fades to insane flashing.

You don't need to build the entire suit to use some of the code.  Other than the Teensy and the LED ribbon, the main other component is a 2x18character LCD display and 4 rotary encoders to control everything.  If you were willing to sacrifice some of the controls, you could replace this with a couple of forward/back buttons.

Hardware:
  Teensy 3.0  (3.1 might also work)

Build with:
  Arduino 1.65+
  Teensduino 1.24+

It can also be built with VisualMicro on VisualStudio 2013, which is a much better development environment than trying to use the Arduino IDE.
  
Required libraries are in "_lib", these will need to be copied to your Arduino system library location  
 