/*========================================================*/
/*
  uProject #2 "Pushing the Right Buttons"
  by Judy Kong, HCII CMU. Sep 10, 2019.
  
  This is the second micro project assignment for 05-833 Applied Gadgets, 
  Sensors and Activity Regocnition in HCI. 
  
  Hardware
  This code assumes four LEDs attached to digital I/O pin 2, 3, 4, 5.
  The LEDs are assumed to be connected as common cathode attached to 
  ground and should should each have a current limiting resistor (probably in 
  the 200-500 ohm range for 5v hardware). This code also assumes two 
  push button swtiches attached to digital I/O pin 6, 7.
  
  Operation
  When one button is pressed, the LEDs should light in sequence left to 
  right (and then repeat).  When the other is pressed the LEDs should switch 
  directions and light in sequence right to left (and then repeat).
  
*/
/*========================================================*/

/*------- Hardware configuration -------*/
const int  PinA = 2;
const int  PinB = 3;
const int  PinC = 4;
const int  PinD = 5;
const int  switchPinLR = 6;        // pin with switch attached
const int  switchPinRL = 7;        // pin with switch attached
const byte debugPin = 13;        // pin that we put debug output on (set to 255 to disable)
                                 // (most Arduino's have a built in LED on pin 13...)

/*------- Data structures for system state -------*/      

/* Structure to track and debounce the state of a push button switch */
typedef struct switchTracker {
  int lastReading;       // last raw value read
  long lastChangeTime;   // last time the raw value changed
  byte pin;              // the pin this is tracking changes on
  byte switchState;      // debounced state of the switch
} switchTrack;

/* Fill in a switchTrack structure to start tracking/debouncing a switch
   on the given pin.
*/
void initSwitchTrack(struct switchTracker &sw, int swPin) {  
  pinMode(swPin, INPUT);
  sw.lastReading = digitalRead(swPin);
  sw.lastChangeTime = millis();
  sw.pin = swPin;
  sw.switchState = sw.lastReading;
}

// Structure representing the color and duration of one flicker segment
unsigned int flashDur = 250;      // duration in msec
unsigned int flickDur = 1000;      // duration in msec

/*------- Global system state -------*/
const byte LEFT_TO_RIGHT = 0;    // flash the 4 LED's from left to right
const byte RIGHT_TO_LEFT = 1;    // flash the 4 LED's from right to left
byte flameMode = LEFT_TO_RIGHT;
switchTrack switchInputLR;     // tracking / debounce information for our switch
switchTrack switchInputRL;     // tracking / debounce information for our switch

/*-------------------------------------------*/
/* Flash the debug LED for the given time in msec.  The LED is on for flashMSec then held
   off for the same amount of time (so this routine takes 2*flashMSec total).
 */
void dbgFlash(int flashMSec) {
  if (debugPin != 255) {
    digitalWrite(debugPin,HIGH); delay(flashMSec); 
    digitalWrite(debugPin,LOW); delay(flashMSec);
  }
}

/*-------------------------------------------*/
/* Initializization code (run once via call from Arduino framework) */
void setup() {
  // establish direction of pins we are using to drive LEDs 
  pinMode(PinA, OUTPUT); 
  pinMode(PinB, OUTPUT); 
  pinMode(PinC, OUTPUT); 
  pinMode(PinD, OUTPUT); 
  pinMode(debugPin, OUTPUT);

  // set up debounce tracker on our switch input pin (sets mode to INPUT)
  initSwitchTrack(switchInputLR, switchPinLR);
  initSwitchTrack(switchInputRL, switchPinRL);
}

/* Main routine (called repeated by from the Arduino framework) */
void loop() {
  flick();
}

/*-------------------------------------------*/
/* Using a time based debounce, test whether a switch should be considered to have 
   changed state since the last call to this routine for the switch, and track the 
   current state of the switch in the given switch tracking structure (sw).  The switch 
   will be consided to be in a new state only after it has held that state for debounceTime msec 
   or longer.  sw.switchState will be set to 1 when the switch should be considered pressed 
   and 0 when it should be considered released.  This routine will return true when the 
   switch is in a new state compared to the last call to this routine, false otherwise.
   
   Note: this routine must be called frequently to operate correctly.
   Note: this code is here to make the demo a little more stand alone (nothing to import),
         but in other cases you might want to consider using the code from Bounce.h instead.
*/
boolean switchChange(struct switchTracker &sw) { 
  // switch must stay stable this long (in msec) to register
  const long debounceTime = 100;   
  
  // default to no change until we find out otherwise
  boolean result = false; 
  
  // get the current raw reading from the switch
  int reading = digitalRead(sw.pin);

  // if the raw reading has changed (by noise or actual press/release) reset change time to now
  if (reading != sw.lastReading) sw.lastChangeTime = millis();
  sw.lastReading = reading;
  
  // if time since the last change is longer than the required dwell
  if ((millis() - sw.lastChangeTime) > debounceTime) {
    // note whether we are changing
    result = (reading != sw.switchState);
    // in any case the value has been stable and so the reported state 
    // should now match the current raw reading
    sw.switchState = reading;
  }
  return result;
}

/*-------------------------------------------*/
/* Routine to do sequential flashing depending on flame mode. 
 */
void flick(void) {  

  // unsigned long startTime = millis();
  // while (millis() - startTime < flickDur) {
    // Count out flash duration by millisecond
    for (int cnt = 0; cnt < flashDur * 4; cnt++) {
      // Flash the lights
      if (flameMode == LEFT_TO_RIGHT) {
        flashLR(cnt);
      } else {
        flashRL(cnt);
      }
      // see if the switch has been pressed
      if (switchChange(switchInputLR) && switchInputLR.switchState == 1 /* pressed */) {
        // Switch to left-to-right mode
        flameMode = LEFT_TO_RIGHT;
        dbgFlash(50);  // debug flash so we know the switch input is working right
        goto done;  
      } else if (switchChange(switchInputRL) && switchInputRL.switchState == 1 /* pressed */) {
        // Switch to right-to-left mode
        flameMode = RIGHT_TO_LEFT;
        dbgFlash(50);  // debug flash so we know the switch input is working right
        goto done;  
      }
      delay(1);
    } // end for
  // } // end while
  
  done:
  // turn off all LEDs
  digitalWrite(PinA,LOW);
  digitalWrite(PinB,LOW);
  digitalWrite(PinC,LOW);
  digitalWrite(PinD,LOW);
}

/*-------------------------------------------*/
/* Flash from the left to the right.
 */
void flashLR(int cnt) {
  if (cnt <= flashDur) {
    digitalWrite(PinA, HIGH);digitalWrite(PinB, LOW); 
    digitalWrite(PinC, LOW); digitalWrite(PinD, LOW);
  } else if (cnt <= flashDur * 2) {
    digitalWrite(PinA, LOW); digitalWrite(PinB, HIGH);
    digitalWrite(PinC, LOW); digitalWrite(PinD, LOW);
  } else if (cnt <= flashDur * 3) {
    digitalWrite(PinA, LOW); digitalWrite(PinB, LOW);
    digitalWrite(PinC, HIGH);digitalWrite(PinD, LOW);
  } else {
    digitalWrite(PinA, LOW); digitalWrite(PinB, LOW);
    digitalWrite(PinC, LOW); digitalWrite(PinD, HIGH);
  }
}

/*-------------------------------------------*/
/* Flash from the right to the left.
 */
void flashRL(int cnt) {
  if (cnt <= flashDur) {
    digitalWrite(PinA, LOW); digitalWrite(PinB, LOW);
    digitalWrite(PinC, LOW); digitalWrite(PinD, HIGH);
  } else if (cnt <= flashDur * 2) {
    digitalWrite(PinA, LOW); digitalWrite(PinB, LOW);
    digitalWrite(PinC, HIGH);digitalWrite(PinD, LOW);
  } else if (cnt <= flashDur * 3) {
    digitalWrite(PinA, LOW); digitalWrite(PinB, HIGH);
    digitalWrite(PinC, LOW); digitalWrite(PinD, LOW);
  } else {
    digitalWrite(PinA, HIGH);digitalWrite(PinB, LOW);
    digitalWrite(PinC, LOW); digitalWrite(PinD, LOW);
  }
}
