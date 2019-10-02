/*========================================================*/
/*
  Project #1 "I'm OK (You're OK)"
  by Judy Kong, HCII CMU. Oct 1, 2019.
  
  This is the checkpoint for the first project for 05-833 Applied Gadgets, 
  Sensors and Activity Regocnition in HCI. 
  

  Reference
  This code uses part of the configuration in 
  https://create.arduino.cc/projecthub/SAnwandter1/programming-8x8-led-matrix-23475a
  
*/
/*========================================================*/

/*------- Hardware configuration -------*/
#define ROW_1 4
#define ROW_2 5
#define ROW_3 7
#define ROW_4 9
#define ROW_5 8
#define ROW_6 10
#define ROW_7 11
#define ROW_8 6

#define COL_1 A2
#define COL_2 A0
#define COL_3 A3
#define COL_4 A1
#define COL_5 A5
#define COL_6 3
#define COL_7 A4
#define COL_8 2

const byte rows[] = {
  ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7, ROW_8
};
const byte cols[] = {
  COL_1,COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8
};

byte O[] = {0x7E, 0xE7, 0xC3, 0xC3, 0xC3, 0xC3, 0xE7, 0x7E};
byte K[] = {0x63, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0x63, 0x61};
byte YOU_WIN[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
byte GAME_OVER[] = {0x00, 0x00, 0x66, 0x66, 0x18, 0x24, 0x42, 0x00};

const int  switchPinRight = 12;     // pin with switch attached
const int  switchPinLeft = 13;      // pin with switch attached
const byte debugPin = 13;           // pin that we put debug output on (set to 255 to disable)
                                    // (most Arduino's have a built in LED on pin 13...)

unsigned long seed;
int timeCount;

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
unsigned int flickDur = 500;     // duration in msec

/*------- Global system state -------*/
const int winningThreshold = 20;
const byte TURN_LEFT = 0;         // Turn left
const byte TURN_RIGHT = 1;        // Turn right
const byte LEFT = 0;
const byte UP = 1;
const byte RIGHT = 2;
const byte DOWN = 3;
const byte RUNNING = 0;
const byte WON = 1;
const byte DEAD = 2;
byte currConfig[] = {0x00, 0x00, 0x00, 0xF2, 0x00, 0x00, 0x00, 0x00};
byte directionMode = RIGHT;
byte positionI[64];
byte positionJ[64];
byte currLength = 4;
byte currStatus = RUNNING;
byte targetI = 4;                 // Initial target position i
byte targetJ = 6;                 // Initial target position j
switchTrack switchInputLeft;      // tracking / debounce information for our switch
switchTrack switchInputRight;     // tracking / debounce information for our 

/*-------------------------------------------*/
/* Initializization code (run once via call from Arduino framework) */
void setup() {

  randomSeed(seed);

  // Digital Pins
  // establish direction of pins we are using to drive LEDs 
  for (int i = 2; i < 12; i++) {
    pinMode(i, OUTPUT);
  }
  
  // Analog Pins
  for (int i = 14; i < 22; i++) {
    pinMode(i, OUTPUT);
  }

  timeCount = 0;

  // Initialize Snake
  for (int i = 0; i < 4; i++) {
    positionI[i] = 4;
    positionJ[i] = 4 - i;
  }
  
  // set up debounce tracker on our switch input pin (sets mode to INPUT)
  initSwitchTrack(switchInputLeft, switchPinLeft);
  initSwitchTrack(switchInputRight, switchPinRight);
  
}

/* Main routine (called repeated by from the Arduino framework) */
void loop() {
  
  for (int cnt = 0; cnt < flickDur; cnt++) {

    if (currStatus == RUNNING) {
      drawScreen(currConfig);
    } else if (currStatus == WON) {
      drawScreen(YOU_WIN);
    } else {
      drawScreen(GAME_OVER);
    }
    
    // see if the switch has been pressed
    if (switchChange(switchInputLeft) && switchInputLeft.switchState == 1 /* pressed */) {
      // Turn left
      directionMode = (directionMode - 1) % 4;
      // dbgFlash(50);  // debug flash so we know the switch input is working right
      goto done;  
    } else if (switchChange(switchInputRight) && switchInputRight.switchState == 1 /* pressed */) {
      // Turn right
      directionMode = (directionMode + 1) % 4;
      // dbgFlash(50);  // debug flash so we know the switch input is working right
      goto done;
    }
  }

  done:
  proceed();
  refresh();
  target();

}

void drawScreen(byte buffers[]) { 
  // Turn on each row in series
  for (byte i = 0; i < 8; i++) {
    digitalWrite(rows[i], LOW);    //initiate whole row
    for (byte j = 0; j < 8; j++) {
      byte currPixel = (buffers[i] >> (7 - j)) & 0x1;
      if (i + 1 == targetI && j + 1 == targetJ) currPixel = 1;
      digitalWrite(cols[j], currPixel); // initiate whole column
      digitalWrite(cols[j], 0);      // reset whole column
    }
    digitalWrite(rows[i], HIGH);     // reset whole row
  }
}

boolean switchChange(struct switchTracker &sw) { 
  
  const long debounceTime = 50; // switch must stay stable this long (in msec) to register
  boolean result = false; // default to no change until we find out otherwise

  int reading = digitalRead(sw.pin); // get the current raw reading from the switch
  // if the raw reading has changed (by noise or actual press/release) reset change time to now
  if (reading != sw.lastReading) sw.lastChangeTime = millis();
  sw.lastReading = reading;
  
  // if time since the last change is longer than the required dwell
  if ((millis() - sw.lastChangeTime) > debounceTime) {
    result = (reading != sw.switchState); // note whether we are changing
    sw.switchState = reading;
  }
  
  return result;
}

void proceed() {
  for (int pos = currLength - 1; pos >= 1; pos--) { // Move forward by 1 for every pixel
    positionI[pos] = positionI[pos - 1];
    positionJ[pos] = positionJ[pos - 1];
  }
  if (directionMode == LEFT) {
    positionJ[0] = positionJ[0] - 1;
  } else if (directionMode == UP) {
    positionI[0] = positionI[0] - 1;
  } else if (directionMode == RIGHT) {
    positionJ[0] = positionJ[0] + 1;
  } else { // assert directionMode == DOWN
    positionI[0] = positionI[0] + 1;
  }
  byte newI = positionI[0];
  byte newJ = positionJ[0];
  if (newI == targetI and newJ == targetJ) {
    currLength += 1;
  }
  if (currLength == winningThreshold) {
    currStatus = WON;
    return;
  }
  if (newI <= 0 or newI >= 9 or newJ <= 0 or newJ >= 9) {
    currStatus = DEAD;
    return;
  }
  if (positionI[currLength - 1] == newI and positionJ[currLength - 1] == newJ) {
    currStatus = DEAD;
    return;
  }
}

void refresh() {
  memset(currConfig, 0, sizeof(currConfig));
  for (int pos = 1; pos < currLength; pos++) {
    byte i = positionI[pos] - 1;
    byte j = positionJ[pos] - 1;
    currConfig[i] = currConfig[i] | (1 << (7 - j));;
  }
  byte i = positionI[0] - 1;
  byte j = positionJ[0] - 1;
  if ((currConfig[i] >> (7 - j)) & 0x1) {
    currStatus = DEAD;
  } else {
    currConfig[i] = currConfig[i] | (1 << (7 - j));;
  }
}

void target() {
  if (positionI[0] != targetI or positionJ[0] != targetJ) return;
  while ((currConfig[targetI - 1] >> (8 - targetJ)) & 0x1) {
    targetI = random(1, 9);
    targetJ = random(1, 9);
  }
}
