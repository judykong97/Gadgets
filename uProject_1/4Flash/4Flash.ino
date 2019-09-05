/*========================================================*/
/*
  LED Candle Demo v0.2c (Non-Candle Starter Version)
  by Scott Hudson, HCI Institute, CMU 1/10
  
  This program provides a demo of basic techniques (push button input and
  driving some LEDs) in the form of a simulated candle created with an RGB
  LED.  For this Starter version we just light the LEDs in a fixed pattern 
  for testing.
  
  Hardware
  This code assumes three LEDs or a combined RBG LED. Each of the LEDs should 
  be attached to a separate digital I/O pin (as indicated in the constants 
  below).  The LEDs are assumed to be connected as common cathode attached to 
  ground and should should each have a current limiting resistor (probably in 
  the 200-500 ohm range for 5v hardware). 
  
  Operation
  This simplified version of the program just lights the LEDs in a recognizable pattern
  for initial testing.
  
  Revisions
  1/10  Initial version v0.1a
  1/10  Split into 3 progressively more complete versions v0.2a 
  1/17  Change pin constants for new part pinout v0.2b 
  8/19  Change pin constants for another new part pinout v0.2c
*/
/*========================================================*/

/*------- Hardware configuration -------*/
const int  PinA = 2;
const int  PinB = 3;
const int  PinC = 4;
const int  PinD = 5;
const byte debugPin = 13;        // pin that we put debug output on (set to 255 to disable)
                                 // (most Arduino's have a built in LED on pin 13...)
  
/*-------------------------------------------*/
/* Initializization code (run once via call from Arduino framework) */
void setup() {
  // establish direction of pins we are using to drive LEDs 
  pinMode(PinA, OUTPUT); 
  pinMode(PinB, OUTPUT); 
  pinMode(PinC, OUTPUT); 
  pinMode(PinD, OUTPUT); 
  pinMode(debugPin, OUTPUT);
}

/* Main routine (called repeated by from the Arduino framework) */
void loop() {
  
    // (OPTIONAL) Debug Pin
    // digitalWrite(debugPin,HIGH); delay(250); digitalWrite(debugPin,LOW);
    
    // 4 Flash
    digitalWrite(PinA,HIGH); delay(2000); digitalWrite(PinA,LOW);
    digitalWrite(PinB,HIGH); delay(1000); digitalWrite(PinB,LOW);
    digitalWrite(PinC,HIGH); delay(500); digitalWrite(PinC,LOW);
    digitalWrite(PinD,HIGH); delay(250); digitalWrite(PinD,LOW);
    
    // All LEDs are now off, stay dark for 1/2 sec
    delay(500);
    
}
