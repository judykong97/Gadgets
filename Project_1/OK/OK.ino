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
  COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8
};

byte O[] = {0x7E, 0xE7, 0xC3, 0xC3, 0xC3, 0xC3, 0xE7, 0x7E};
byte K[] = {0x63, 0x66, 0x6C, 0x78, 0x6C, 0x66, 0x63, 0x61};

const byte debugPin = 13;        // pin that we put debug output on (set to 255 to disable)
                                 // (most Arduino's have a built in LED on pin 13...)

int timeCount;

/*-------------------------------------------*/
/* Initializization code (run once via call from Arduino framework) */
void setup() {
  timeCount = 0;
  // Digital Pins
  // establish direction of pins we are using to drive LEDs 
  for (int i = 2; i < 22; i++) {
    pinMode(i, OUTPUT);
  }
  // Analog Pins
  for (int i = 14; i < 22; i++) {
    pinMode(i, OUTPUT);
  }
  pinMode(debugPin, OUTPUT);
}

/* Main routine (called repeated by from the Arduino framework) */
void loop() {
  
  timeCount += 1;
  
  if (timeCount < 2500) {
    drawScreen(O); 
  } else if (timeCount <  5000) {
    drawScreen(K);
  } else {
    timeCount = 0;
  }

}

void  drawScreen(byte buffers[])
{ 
  // Turn on each row in series
  for (byte i = 0; i < 8; i++) {
    digitalWrite(rows[i], LOW);    //initiate whole row
    for (byte j = 0; j < 8; j++) {
      digitalWrite(cols[j], (buffers[i] >> (7 - j)) & 0x1); // initiate whole column
      digitalWrite(cols[j], 0);      // reset whole column
    }
    digitalWrite(rows[i], HIGH);     // reset whole row
  }
}
