/*========================================================*/
/*
  uProject #6 "Shake It Up"
  by Judy Kong, HCII CMU. Sep 16, 2019.
  
  This is the third micro project assignment for 05-833 Applied Gadgets, 
  Sensors and Activity Regocnition in HCI. 
  
  Hardware
  This code assumes four LEDs attached to digital I/O pin 2, 3, 4, 5.
  The LEDs are assumed to be connected as common cathode attached to 
  ground and should should each have a current limiting resistor (probably in 
  the 200-500 ohm range for 5v hardware). This code also assumes a turn knob
  at analog I/O pin 2.
  
  Operation
  The four LEDs light one after the other and the speed at which the sequence 
  displays is controled with a potentiometer. When the potentiometer is turned 
  fully counter clockwise the LED sequence proceeds slowly. When the 
  potentiometer is turned in the clockwise direction it proceeds faster 
  and faster.
  
*/
/*========================================================*/

#include <Servo.h>

/*------- Hardware configuration -------*/
Servo servo_test;
const int  motorPin = 9; 
const int  potPin = A4; 
const byte debugPin = 13;        // pin that we put debug output on (set to 255 to disable)
                                 // (most Arduino's have a built in LED on pin 13...)

/*-------------------------------------------*/
/* Initializization code (run once via call from Arduino framework) */
void setup() {
  Serial.begin(9600);
  Serial.println("Hello world!");
  servo_test.attach(motorPin);
  pinMode(debugPin, OUTPUT);
}

/* Main routine (called repeated by from the Arduino framework) */
void loop() {
  Serial.println(analogRead(potPin));
  servo_test.write(map(analogRead(potPin), 0, 1024, 180, 0));
  // delay(400);
}
