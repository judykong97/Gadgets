/*========================================================*/
/*
  Project 1 "An Interactive 8x8 Matrix LED Game: Snake"
  by Judy Kong, HCII CMU. Oct 7, 2019.
  
  This is the first class project for 05-833 Applied Gadgets, 
  Sensors and Activity Regocnition in HCI. 
 
  Hardware
  This code assumes an 8x8 LEDs matrix, with its rows attached to 
  digital I/O pins 4-11 and its rows attached to digital I/O pins 
  2, 3 and analog I/O pins A0-A5 (used as digital I/O pins). 
  Each row should each have a current limiting resistor with 470 ohms. 
  This code also assumes two push button swtiches attached to digital 
  I/O pin 12 and 13.

  Operation
  The snake starts at the middle of the screen moving to the right with
  a starting length of 4. The user uses the two push buttons to 
  control the snake to turn left and right to eat the target on the screen.
  Whenever the snake eats the target, its length grows by 1 and there
  will be a new target generated. 
  The user wins if the length of the snake reaches a certain length; 
  and the snake dies if it crashes into the wall or itself. 
  
  Reference
  This code used part of the configuration in 
  https://create.arduino.cc/projecthub/SAnwandter1/programming-8x8-led-matrix-23475a
  
*/
/*========================================================*/

/*------- Hardware configuration -------*/
#define ROW_1 A5
#define ROW_2 A4
#define ROW_3 A2
#define ROW_4 7
#define ROW_5 A1
#define ROW_6 9
#define ROW_7 8
#define ROW_8 A3

#define COL_1 12
#define COL_2 10
#define COL_3 5
#define COL_4 11
#define COL_5 4
#define COL_6 6
#define COL_7 3
#define COL_8 2

const byte rows[] = {
  ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7, ROW_8
};
const byte cols[] = {
  COL_1,COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8
};

byte YOU_WIN[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};   // turn the whole screen on when user wins
byte GAME_OVER[] = {0x00, 0x00, 0x66, 0x66, 0x18, 0x24, 0x42, 0x00}; // display a sad face when user loses

const int  switchPinLeft = 14;      // pin with the left switch attached
const int  switchPinRight = 13;     // pin with the right switch attached

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
unsigned int flickDur = 600;     // duration in msec

/*------- Global system state -------*/
const int winningThreshold = 127;  // length of snake to win the game
const byte TURN_LEFT = 0;         // turn left
const byte TURN_RIGHT = 1;        // turn right
const byte LEFT = 0;              // running to the left
const byte UP = 1;                // running up
const byte RIGHT = 2;             // running to the right
const byte DOWN = 3;              // running down
const byte RUNNING = 0;           // the snake is running
const byte WON = 1;               // the snake wins
const byte DEAD = 2;              // the snake is dead
// 8x8 display of current snake position
byte currConfig[] = {0x00, 0x00, 0x00, 0xF2, 0x00, 0x00, 0x00, 0x00};   
byte directionMode = RIGHT;       // current direction the snake is running
byte positionI[64];               // current position i of the head of snake
byte positionJ[64];               // current position j of the head of snake
byte currLength = 4;              // current length of snake
byte currStatus = RUNNING;        // current status of snake
byte targetI = 4;                 // initial target position i is in the middle row
byte targetJ = 7;                 // initial target position j is in the 4th column
switchTrack switchInputLeft;      // tracking / debounce information for our switch
switchTrack switchInputRight;     // tracking / debounce information for our 

/*-------------------------------------------*/
/* Initializization code */
void setup() {

  // Initialize random number generator
  randomSeed(analogRead(0));

  // Digital Pins
  // establish direction of pins we are using to drive LEDs 
  for (int i = 2; i < 13; i++) {
    pinMode(i, OUTPUT);
  }
  
  // Analog Pins
  // establish direction of pins we are using to drive LEDs
  for (int i = 15; i < 20; i++) {
    pinMode(i, OUTPUT);
  }

  // Initialize snake position
  for (int i = 0; i < 4; i++) {
    positionI[i] = 4;
    positionJ[i] = 4 - i;
  }
  
  // Set up debounce tracker on our switch input pin (sets mode to INPUT)
  initSwitchTrack(switchInputLeft, switchPinLeft);
  initSwitchTrack(switchInputRight, switchPinRight);
  
}

/* Main routine */
void loop() {

  // Proceed after every flickDur milliseconds
  // Speeds up as the snake grows longer 
  for (int cnt = 0; cnt < flickDur - 20 * currLength; cnt++) {

    // Display depending on current status of the snake
    if (currStatus == RUNNING) { 
      drawScreen(currConfig); // display snake position when it's running
    } else if (currStatus == WON) {
      drawScreen(YOU_WIN); // display YOU WIN when user wins
      return; 
    } else {
      drawScreen(GAME_OVER); // display GAME OVER when snake dies
      return; 
    }
    
    // See if the switch has been pressed
    if (switchChange(switchInputLeft) && switchInputLeft.switchState == 1 /* pressed */) {
      // Turn left
      directionMode = (directionMode - 1) % 4;
      goto done;  
    } else if (switchChange(switchInputRight) && switchInputRight.switchState == 1 /* pressed */) {
      // Turn right
      directionMode = (directionMode + 1) % 4;
      goto done;
    }
  }

  done:
  proceed();      // Let the snake move one pixel forward in its direction
  refresh();      // Refresh the 8x8 LED matrix display
  target();       // Check for target and regenerate target if needed

}

/*-------------------------------------------*/
/* Turn on the 8x8 LED matrix row by row based on current configuration of the matrix.
*/
void drawScreen(byte buffers[]) {
  // Turn on each row in series
  for (byte i = 0; i < 8; i++) {
    digitalWrite(rows[i], LOW); // initiate whole row
    for (byte j = 0; j < 8; j++) {
      byte currPixel = (buffers[i] >> (7 - j)) & 0x1;
      if (currStatus == RUNNING && i + 1 == targetI && j + 1 == targetJ) 
        currPixel = 1;
      digitalWrite(cols[j], currPixel); // initiate whole column
      digitalWrite(cols[j], 0); // reset whole column
    }
    digitalWrite(rows[i], HIGH); // reset whole row
  }
}

/*-------------------------------------------*/
/* Using a time based debounce, test whether a switch should be considered to have 
   changed state since the last call to this routine for the switch, and track the 
   current state of the switch in the given switch tracking structure (sw).  The switch 
   will be consided to be in a new state only after it has held that state for debounceTime msec 
   or longer.  sw.switchState will be set to 1 when the switch should be considered pressed 
   and 0 when it should be considered released. This routine will return true when the 
   switch is in a new state compared to the last call to this routine, false otherwise.
*/
boolean switchChange(struct switchTracker &sw) { 
  
  const long debounceTime = 10; // switch must stay stable this long (in msec) to register
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

/*-------------------------------------------*/
/* Let the snake move forward by 1 pixel in its direction. If the snake reaches a certain 
 * length, the user wins; if the snake crashes into the wall or into itself, then it dies. 
*/
void proceed() {
  
  // Move forward by 1 for every pixel
  for (int pos = currLength - 1; pos >= 1; pos--) { 
    positionI[pos] = positionI[pos - 1];
    positionJ[pos] = positionJ[pos - 1];
  }
  
  // Set the new head position
  if (directionMode == LEFT) {
    positionJ[0] = positionJ[0] - 1; // move to the left
  } else if (directionMode == UP) {
    positionI[0] = positionI[0] - 1; // move up
  } else if (directionMode == RIGHT) {
    positionJ[0] = positionJ[0] + 1; // move to the right 
  } else { // assert directionMode == DOWN
    positionI[0] = positionI[0] + 1; // move down
  }
  
  byte newI = positionI[0];
  byte newJ = positionJ[0];
  // If the snake eat the target, grow the length of snake by 1
  if (newI == targetI and newJ == targetJ) {
    currLength += 1;
  }
   // If snake length reaches winning threshold, set the status to WON
  if (currLength == winningThreshold) {
    currStatus = WON;
    return;
  }
  // If snake crashes into the wall, set the status to DEAD
  if (newI <= 0 or newI >= 9 or newJ <= 0 or newJ >= 9) {
    currStatus = DEAD;
    return;
  }
//  if (positionI[currLength - 1] == newI and positionJ[currLength - 1] == newJ) {
//    currStatus = DEAD;
//    return;
//  }
}

/*-------------------------------------------*/
/* Refresh the 8x8 LED matrix display based on the new snake position
*/
void refresh() {

  // If the user already wins or the snake is dead, return directly 
  if (currStatus == WON or currStatus == DEAD) return;
  
  // Reset display
  memset(currConfig, 0, sizeof(currConfig));
  
  // Set (i, j) of configuration from 0 to 1 if the snake is in the position
  for (int pos = 1; pos < currLength; pos++) {
    byte i = positionI[pos] - 1;
    byte j = positionJ[pos] - 1;
    currConfig[i] = currConfig[i] | (1 << (7 - j));;
  }
  
  byte i = positionI[0] - 1;
  byte j = positionJ[0] - 1;
  // Check if the snake crashes into itself
  if ((currConfig[i] >> (7 - j)) & 0x1) {
    currStatus = DEAD; // if so, set the status to DEAD
  } else {
    // if not, set the new head position to 1 in the configuration
    currConfig[i] = currConfig[i] | (1 << (7 - j));
  }
}

/*-------------------------------------------*/
/* Check if the target has been eaten. If so, regenerate the target. 
 * To make sure the target is on top of the snake, keep regenerating
 * until the target is at a position of value 0 in the configuration,
 * meaning that the snake is not in that pixel. 
*/
void target() {

  // If the user already wins or the snake is dead, return directly
  if (currStatus == WON or currStatus == DEAD) return;
  
  // If the target is not eaten, no need to regenerate
  if (positionI[0] != targetI or positionJ[0] != targetJ) return;

  // Keeps regenerating until the target is not on the snake
  while ((currConfig[targetI - 1] >> (8 - targetJ)) & 0x1) {
    targetI = random(1, 9);
    targetJ = random(1, 9);
  }
}
