/*========================================================*/
/*
  uProject #4 "Blinded by the Light"
  by Judy Kong, HCII CMU. Nov 10, 2019.
  
  This is the fourth micro project assignment for 05-833 Applied Gadgets, 
  Sensors and Activity Regocnition in HCI. 
  
  Hardware
  This code assumes four LEDs attached to digital I/O pin 2, 3, 4, 5.
  The LEDs are assumed to be connected as common cathode attached to 
  ground and should should each have a current limiting resistor (probably in 
  the 200-500 ohm range for 5v hardware). This code also assumes a photocell
  at analog I/O pin 0.
  
  Operation
  If the light (or temperature) is “low” no LEDs will be lit, and as
  the light (or temperature) increases more LEDs should be turned on. 
  
*/
/*========================================================*/

/*------- Hardware configuration -------*/
const int  PinA = 2;
const int  PinB = 3;
const int  PinC = 4;
const int  PinD = 5;
int photocellPin = A0;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the sensor divider
int brightnessLevel;
const byte debugPin = 13;        // pin that we put debug output on (set to 255 to disable)
                                 // (most Arduino's have a built in LED on pin 13...)

/*------- Knob reader -------*/
void dynamicDelay(int angle1024) {
  delay((250 + angle1024));
}

/*-------------------------------------------*/
/* Initializization code (run once via call from Arduino framework) */
void setup() {

  Serial.begin(9600);
  
  // establish direction of pins we are using to drive LEDs 
  pinMode(PinA, OUTPUT); 
  pinMode(PinB, OUTPUT); 
  pinMode(PinC, OUTPUT); 
  pinMode(PinD, OUTPUT); 
  pinMode(debugPin, OUTPUT);
}

/* Main routine (called repeated by from the Arduino framework) */
void loop() {

  photocellReading = analogRead(photocellPin);  
 
  Serial.print("Analog reading = ");
  Serial.println(photocellReading);     // the raw analog reading

  if (photocellReading >= 1023) {
    brightnessLevel = 4;
  } else if (photocellReading >= 1020) {
    brightnessLevel = 3;
  } else if (photocellReading >= 1015) {
    brightnessLevel = 2;
  } else if (photocellReading >= 1010) {
    brightnessLevel = 1;
  } else {
    brightnessLevel = 0;
  }

  if (brightnessLevel == 0) {
    digitalWrite(PinA,LOW);
    digitalWrite(PinB,LOW);
    digitalWrite(PinC,LOW);
    digitalWrite(PinD,LOW);
  } else if (brightnessLevel == 1) {
    digitalWrite(PinA,HIGH);
    digitalWrite(PinB,LOW);
    digitalWrite(PinC,LOW);
    digitalWrite(PinD,LOW);
  } else if (brightnessLevel == 2) {
    digitalWrite(PinA,HIGH);
    digitalWrite(PinB,HIGH);
    digitalWrite(PinC,LOW);
    digitalWrite(PinD,LOW);
  } else if (brightnessLevel == 3) {
    digitalWrite(PinA,HIGH);
    digitalWrite(PinB,HIGH);
    digitalWrite(PinC,HIGH);
    digitalWrite(PinD,LOW);
  } else { // brightnessLevel == 4
    digitalWrite(PinA,HIGH);
    digitalWrite(PinB,HIGH);
    digitalWrite(PinC,HIGH);
    digitalWrite(PinD,HIGH);
  }
  delay(200);
}
