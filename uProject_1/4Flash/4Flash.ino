/*========================================================*/
/*
  uProject #1 "Let There Be Light"
  by Judy Kong, HCII CMU. Sep 4, 2019.
  
  This is the first micro project assignment for 05-833 Applied Gadgets, 
  Sensors and Activity Regocnition in HCI. 
  
  Hardware
  This code assumes four LEDs attached to digital I/O pin 2, 3, 4, 5.
  The LEDs are assumed to be connected as common cathode attached to 
  ground and should should each have a current limiting resistor (probably in 
  the 200-500 ohm range for 5v hardware). 
  
  Operation
  This program turns the first LED on for 2 seconds, then the second for 1
  second, then the third for a 1/2 second, and then the fourth for a 1/4 
  second. Then flash the four in some customized pattern. Wait 1/2 second, and 
  then repeat.
  
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

    // Wait for a bit
    delay(500);

    // Customized flashing
    digitalWrite(PinA,HIGH); delay(2000);
    digitalWrite(PinB,HIGH); delay(1000);
    digitalWrite(PinC,HIGH); delay(500);
    digitalWrite(PinD,HIGH); delay(250);
    digitalWrite(PinA,LOW); digitalWrite(PinB,LOW);
    digitalWrite(PinC,LOW); digitalWrite(PinD,LOW);

    // All LEDs are now off, stay dark for 1/2 sec
    delay(500);
    
}
